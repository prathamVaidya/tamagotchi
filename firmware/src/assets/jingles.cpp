// jingles.cpp — a short sound for each face expression (see jingles.h).

#include "jingles.h"

#include <stddef.h>

namespace {

using buzzer::Tone;

// Wall-E-style robot voices. Recipe: stuttered short blips for the
// broken-circuit feel + stepped fixed-tone runs (LEDC switches square
// waves instantly, so 10-18 ms steps read as a pitch glide). Pitches
// sit in the 600-2800 Hz range where the passive piezo is loudest.

// NEUTRAL — inquisitive "beep?"
const Tone NEUTRAL[] = {{1175, 50}, {1397, 80}};

// HAPPY — "be-he-bweeeeOOP-he-he-wheep!"
const Tone HAPPY[] = {
    // "be-he!" stutter
    {1245, 35}, {0, 25}, {1245, 35}, {0, 25},
    // up-chirp "bweeeeOOP!" — stepped sweep ~880 → 2349 Hz
    {880, 14}, {1109, 14}, {1397, 14}, {1760, 14}, {2093, 14}, {2349, 60},
    {0, 50},
    // chuckle "he-he-he"
    {1568, 30}, {0, 20}, {1760, 30}, {0, 20}, {1976, 30},
    {0, 35},
    // final "wheep!"
    {1397, 20}, {1760, 20}, {2349, 90},
};

// SAD — drooping "wooo-uhhh... uh uh" — down-chirp then sad stutters
const Tone SAD[] = {
    {1568, 60}, {1397, 60}, {1245, 60}, {1047, 70}, {880, 80}, {740, 110},
    {0, 90},
    {659, 80}, {0, 70}, {587, 80}, {0, 70}, {494, 180},
};

// SLEEPY — long yawning down-chirp ending in a tiny snore
const Tone SLEEPY[] = {
    {1175, 90}, {1047, 90}, {932, 90}, {831, 90}, {740, 90}, {659, 130},
    {0, 160},
    {523, 60}, {0, 50}, {440, 220},
};

// EXCITED — climbing stutter "beep-beep-beep-WHEEP!"
const Tone EXCITED[] = {
    {1397, 35}, {0, 25}, {1568, 35}, {0, 25}, {1760, 35}, {0, 25},
    {1976, 35}, {0, 25}, {2093, 40}, {0, 35},
    // final whoop chirp up
    {1760, 14}, {1976, 14}, {2217, 14}, {2489, 14}, {2794, 90},
};

// SURPRISED — sharp upward chirp "WHEEP?!"
const Tone SURPRISED[] = {
    {880, 10}, {1175, 10}, {1568, 10}, {1976, 10}, {2349, 10}, {2794, 80},
    {0, 35},
    {2349, 30}, {2794, 110},
};

// ANGRY — harsh agitated warble between two clashing pitches
const Tone ANGRY[] = {
    {1976, 55}, {1397, 55}, {1976, 55}, {1397, 55},
    {2093, 55}, {1480, 55}, {2093, 55}, {1480, 55},
    {2217, 110},
};

// BLINK — tiny circuit tick (kept short to match the eyelid flick)
const Tone BLINK[] = {{1568, 22}, {1976, 28}};

// LOVE — swoony rising chirp, soft "aww" sigh, little heart blip
const Tone LOVE[] = {
    {880, 30}, {1109, 30}, {1397, 40}, {1760, 90},
    {0, 90},
    {1568, 60}, {1397, 60}, {1245, 60}, {1109, 130},
    {0, 70}, {1976, 50},
};

// HORNY — glitched wolf-whistle: up-chirp, down-chirp, cheeky chuckle
const Tone HORNY[] = {
    {880, 18}, {1109, 18}, {1397, 18}, {1760, 18}, {2217, 18}, {2489, 90},
    {0, 50},
    {2489, 18}, {2093, 18}, {1760, 18}, {1397, 18}, {1109, 90},
    {0, 40},
    {1568, 30}, {0, 30}, {1568, 30},
};

// SHY — half-formed chirp that retreats into a tiny apologetic "eep"
const Tone SHY[] = {
    {1397, 30}, {1568, 30}, {1397, 40},
    {0, 100},
    {1175, 60}, {1397, 40},
};

// DEAD — long descending wail with a glitching flicker at the end
const Tone DEAD[] = {
    {1568, 100}, {1397, 100}, {1245, 100}, {1109, 100}, {880, 150}, {659, 220},
    {0, 90},
    {440, 40}, {0, 35}, {440, 40}, {0, 35}, {349, 70}, {0, 60}, {294, 280},
};

template <size_t N>
Jingle make(const Tone (&tones)[N]) {
  return Jingle{tones, static_cast<uint8_t>(N)};
}

}  // namespace

Jingle jingleFor(ExpressionId id) {
  switch (id) {
    case ExpressionId::Happy:
      return make(HAPPY);
    case ExpressionId::Sad:
      return make(SAD);
    case ExpressionId::Sleepy:
      return make(SLEEPY);
    case ExpressionId::Excited:
      return make(EXCITED);
    case ExpressionId::Surprised:
      return make(SURPRISED);
    case ExpressionId::Angry:
      return make(ANGRY);
    case ExpressionId::Blink:
      return make(BLINK);
    case ExpressionId::Love:
      return make(LOVE);
    case ExpressionId::Horny:
      return make(HORNY);
    case ExpressionId::Shy:
      return make(SHY);
    case ExpressionId::Dead:
      return make(DEAD);
    case ExpressionId::Neutral:
    case ExpressionId::Count:
    default:
      return make(NEUTRAL);
  }
}
