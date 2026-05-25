// vibe_test_view.h — music-data readout for VIBE test mode.
//
// Shows what the device is hearing on the SYNC bus *as music*: tempo (BPM),
// position in a 4/4 bar (the active dot walks 1→2→3→4→1…), and a tempo
// label. The dot moving is itself the visible beat — no extra flash needed.
//
// Layout (128x64, default ncenB14 font):
//   ┌──────────────────────────┐
//   │       BPM  128           │  y≈8..15
//   │                          │
//   │   ●    ○    ○    ○       │  y=30, four bar-position dots
//   │                          │
//   │         DANCE            │  y≈56  tempo classification
//   └──────────────────────────┘
#pragma once

#include <stdint.h>

#include "view.h"

class VibeTestView : public View {
 public:
  // Cheap setters; VibeMode pokes these on every loop tick.
  void setBpm(uint16_t bpm) { bpm_ = bpm; }
  void setBeatCount(uint32_t n) { beatCount_ = n; }

  void onEnter() override {}
  void update(uint32_t /*now*/) override {}
  void render(Renderer& r) override;

 private:
  uint16_t bpm_ = 0;
  uint32_t beatCount_ = 0;
};
