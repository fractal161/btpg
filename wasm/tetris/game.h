#pragma once

enum Level {
  kLevel18,
  kLevel19,
  kLevel29,
  kLevel39
};

constexpr int kLineCap = 390;
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
