#pragma once

enum Level {
  kLevel18,
  kLevel19,
  kLevel29,
  kLevel39
};

alignas(32) constexpr float kTransitionProb[][8] = {
  {1./32, 5./32, 6./32, 5./32, 5./32, 5./32, 5./32}, // T
  {6./32, 1./32, 5./32, 5./32, 5./32, 5./32, 5./32}, // J
  {5./32, 6./32, 1./32, 5./32, 5./32, 5./32, 5./32}, // Z
  {5./32, 5./32, 5./32, 2./32, 5./32, 5./32, 5./32}, // O
  {5./32, 5./32, 5./32, 5./32, 2./32, 5./32, 5./32}, // S
  {6./32, 5./32, 5./32, 5./32, 5./32, 1./32, 5./32}, // L
  {5./32, 5./32, 5./32, 5./32, 6./32, 5./32, 1./32}, // I
};

constexpr int kLineCap = 430;
constexpr int kLevelSpeedLines[] = {0, 130, 230, 330, kLineCap};

constexpr int GetLevelByLines(int lines) {
  if (lines < 130) return 18;
  return lines / 10 + 6;
}

constexpr Level GetLevelSpeed(int level) {
  if (level == 18) return kLevel18;
  if (level < 29) return kLevel19;
  if (level < 39) return kLevel29;
  return kLevel39;
}

#ifdef DOUBLE_TUCK
constexpr bool kDoubleTuckAllowed = true;
#else
constexpr bool kDoubleTuckAllowed = false;
#endif
