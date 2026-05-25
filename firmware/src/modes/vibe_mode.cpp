// vibe_mode.cpp — group-sync dance mode (see vibe_mode.h).

#include "vibe_mode.h"

#include <Arduino.h>
#include <esp_random.h>

#include "../assets/expressions.h"
#include "../config.h"
#include "../views/vibe_test_view.h"
#include "../views/view_manager.h"

namespace {

// The dance sequence. Cycled in order, one frame per beat. Chosen to look
// punchy when each step plays the procedural blink-transition — the eye
// snap on every beat reads as a "head bob".
const ExpressionId DANCE[] = {
    ExpressionId::Happy, ExpressionId::Excited,   ExpressionId::Love,    ExpressionId::Excited,
    ExpressionId::Happy, ExpressionId::Surprised, ExpressionId::Excited, ExpressionId::Shy,
};
constexpr uint8_t DANCE_LEN = sizeof(DANCE) / sizeof(DANCE[0]);

// If no external SYNC pulse has been seen for this long, fall back to an
// internal metronome at SOLO_INTERVAL_MS so the device keeps dancing solo.
// As soon as a real pulse arrives the fallback yields again.
constexpr uint32_t SOLO_TIMEOUT_MS = 1500;
constexpr uint32_t SOLO_INTERVAL_MS = 600;  // ≈100 BPM

// In test mode the "HIGH" indicator is held for this long after each rising
// edge so a 2 ms pulse is visible to the human eye at 30 fps.
constexpr uint32_t PIN_HIGH_STRETCH_MS = 120;

// ISR-shared flag: set by the SYNC rising-edge interrupt, cleared by the
// main-loop update(). volatile because the ISR and update() touch it from
// different contexts; a plain bool is fine — we never need atomic RMW.
volatile bool g_beatPending = false;

void IRAM_ATTR onSyncEdge() { g_beatPending = true; }

}  // namespace

VibeMode::VibeMode() = default;

void VibeMode::onEnter(ViewManager& vm) {
  // One-shot setup. We keep the interrupt attached for the lifetime of the
  // program — re-entering Vibe is cheap, and the ISR is harmless (just sets
  // a flag) when not in Vibe Mode.
  if (!initialized_) {
    pinMode(PIN_SYNC, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PIN_SYNC), onSyncEdge, RISING);
    phaseOffset_ = static_cast<uint8_t>(esp_random() % DANCE_LEN);
    initialized_ = true;
  }

  uint32_t now = millis();
  lastBeatMs_ = now;
  lastExtBeatMs_ = now;  // suppress the solo fallback for SOLO_TIMEOUT_MS
  pinHighUntilMs_ = 0;
  pulseCount_ = 0;        // fresh telemetry on each entry
  g_beatPending = false;  // drop any edges queued while we weren't running

  if (testMode_) {
    // Music readout: tempo, bar position, tempo label. No dance.
    VibeTestView& v = vm.vibeTest();
    v.setBpm(0);
    v.setBeatCount(0);
    vm.setView(&v);
  } else {
    danceIdx_ = phaseOffset_;
    vm.setView(&vm.face());
    vm.face().setExpression(DANCE[danceIdx_]);
  }
}

void VibeMode::update(uint32_t now, ViewManager& vm) {
  bool externalBeat = false;
  bool beat = false;

  if (g_beatPending) {
    g_beatPending = false;
    lastExtBeatMs_ = now;
    pinHighUntilMs_ = now + PIN_HIGH_STRETCH_MS;
    pulseCount_++;
    externalBeat = true;
    beat = true;
  } else if (!testMode_ && now - lastExtBeatMs_ > SOLO_TIMEOUT_MS &&
             now - lastBeatMs_ >= SOLO_INTERVAL_MS) {
    // Solo fallback metronome — only runs in dance mode, and only once the
    // external bus has been quiet for SOLO_TIMEOUT_MS so a live DJ always
    // wins. Test mode never invents beats — it must stay honest.
    beat = true;
  }

  if (testMode_) {
    // Music data only: tempo + bar position. The bar-position dot moving
    // 1→2→3→4→1… is the visible beat indicator, so no separate flash.
    VibeTestView& v = vm.vibeTest();
    v.setBeatCount(pulseCount_);
    if (externalBeat) {
      uint32_t gap = now - lastBeatMs_;
      // Plausible musical range; outside it (cold start, sparse noise),
      // leave the BPM where it was — the stale-clear branch handles
      // long silences.
      if (gap >= 100 && gap <= 4000) {
        v.setBpm(static_cast<uint16_t>(60000 / gap));
      }
      lastBeatMs_ = now;
    } else if (now - lastBeatMs_ > 4000) {
      // Long silence: clear the BPM so the screen drops back to "READY".
      v.setBpm(0);
    }
    return;
  }

  if (beat) {
    lastBeatMs_ = now;
    advance_(vm);
  }
}

void VibeMode::onExit() {
  // Leave the interrupt attached. It costs nothing while idle and means
  // re-entry is instant. Pending edges are cleared in onEnter().
}

void VibeMode::advance_(ViewManager& vm) {
  danceIdx_ = (danceIdx_ + 1) % DANCE_LEN;
  vm.face().setExpression(DANCE[danceIdx_]);
}
