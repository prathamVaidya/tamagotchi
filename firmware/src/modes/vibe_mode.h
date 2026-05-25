// vibe_mode.h — group-sync dance mode (see DJ-Plan.md).
//
// Vibe Mode turns the pet into a dancer on a shared bus: every rising edge
// on PIN_SYNC advances one frame in a fixed dance sequence, so a table of
// tamagotchis driven by the same SYNC line all dance in lockstep. Each
// device picks a small random phase offset at boot, so a crowd of devices
// shows a *spread* of frames at any moment instead of the same one — looks
// like a crowd dancing, not the Borg.
//
// When no SYNC pulse has arrived for a short while, Vibe Mode falls back
// to an internal ~100 BPM metronome so a solo device still looks alive
// (useful for testing without the DJ rig wired up).
//
// Entered by the host with `VIBE` or `VIBE on`; any other command kicks
// the main loop back to Desktop Mode, which also exits Vibe.
#pragma once

#include <stdint.h>

#include "mode.h"

class VibeMode : public Mode {
 public:
  VibeMode();

  // Toggle the diagnostic test view (live GPIO 0 telemetry instead of
  // dance frames). Call before setMode() in the main routing so the right
  // view is picked on onEnter(). The flag persists across re-entries.
  void setTestMode(bool on) { testMode_ = on; }

  void onEnter(ViewManager& vm) override;
  void update(uint32_t now, ViewManager& vm) override;
  void onExit() override;

 private:
  void advance_(ViewManager& vm);

  uint8_t danceIdx_ = 0;         // current position in the dance sequence
  uint8_t phaseOffset_ = 0;      // per-device offset, picked once at first entry
  uint32_t lastBeatMs_ = 0;      // last time a beat fired (SYNC or fallback)
  uint32_t lastExtBeatMs_ = 0;   // last time an external SYNC pulse fired
  uint32_t pulseCount_ = 0;      // total SYNC edges seen (test-view only)
  uint32_t pinHighUntilMs_ = 0;  // stretch the visible HIGH state past the 2 ms pulse
  bool initialized_ = false;     // pin + ISR configured?
  bool testMode_ = false;        // VIBE test → telemetry view
};
