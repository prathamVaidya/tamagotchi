// vibe_test_view.cpp — see vibe_test_view.h.

#include "vibe_test_view.h"

#include <stdio.h>

#include "../renderer.h"

namespace {

// Tempo label by BPM bracket. Names are deliberately short so they center
// cleanly in the default 14-px font.
const char* tempoLabel(uint16_t bpm) {
  if (bpm == 0) return "READY";
  if (bpm < 60) return "DRIFT";
  if (bpm < 90) return "SLOW";
  if (bpm < 110) return "GROOVE";
  if (bpm < 130) return "DANCE";
  if (bpm < 160) return "PUMP";
  return "RACE";
}

// Bar-position dots. Four columns evenly spaced across the panel, all on
// the same row in the middle band.
constexpr int DOT_Y = 30;
constexpr int DOT_X[4] = {20, 52, 84, 116};
constexpr int DOT_R_ACTIVE = 5;
constexpr int DOT_R_INACTIVE = 2;

// Pick which dot is "current" given the beat count. Assumes 4/4 — fine for
// the overwhelming majority of what a DJ will be playing. Returns -1 before
// the first beat so the screen starts neutral instead of asserting beat 1.
int activeDot(uint32_t beatCount) {
  if (beatCount == 0) return -1;
  return static_cast<int>((beatCount - 1) % 4);
}

// Draw `s` centered horizontally with baseline at `baselineY`.
void drawCentered(Renderer& r, const char* s, int baselineY) {
  int w = r.textWidth(s);
  int x = (128 - w) / 2;
  if (x < 0) x = 0;
  r.drawText(x, baselineY, s);
}

}  // namespace

void VibeTestView::render(Renderer& r) {
  char buf[16];

  // --- Top: BPM ---------------------------------------------------------
  if (bpm_ > 0 && bpm_ < 1000) {
    snprintf(buf, sizeof(buf), "BPM %u", static_cast<unsigned>(bpm_));
  } else {
    snprintf(buf, sizeof(buf), "BPM --");
  }
  drawCentered(r, buf, 14);

  // --- Middle: bar-position dots ---------------------------------------
  int active = activeDot(beatCount_);
  for (int i = 0; i < 4; i++) {
    int radius = (i == active) ? DOT_R_ACTIVE : DOT_R_INACTIVE;
    r.fillCircle(DOT_X[i], DOT_Y, radius);
  }

  // --- Bottom: tempo label ---------------------------------------------
  drawCentered(r, tempoLabel(bpm_), 58);
}
