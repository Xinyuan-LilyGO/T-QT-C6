#include "CursorEngine.h"

#include <cmath>

namespace input {

bool CursorEngine::update(const drivers::GyroSample& gyro, uint32_t nowMs,
                          int8_t& dx, int8_t& dy) {
  if (lastUpdateMs_ == 0) {
    lastUpdateMs_ = nowMs;
    dx = 0;
    dy = 0;
    return false;
  }

  const uint32_t elapsedMs = nowMs - lastUpdateMs_;
  lastUpdateMs_ = nowMs;
  const float deltaSeconds = min(elapsedMs * 0.001F, kMaxDeltaSeconds);

  // Both physical cursor axes were reversed with the previous mapping.
  const float mappedX = gyro.zDps;
  const float mappedY = gyro.xDps;

  const float alpha =
      1.0F - expf(-deltaSeconds / kFilterTimeConstantSeconds);
  filteredX_ += alpha * (mappedX - filteredX_);
  filteredY_ += alpha * (mappedY - filteredY_);

  residualX_ += response(filteredX_, xActive_) * deltaSeconds;
  residualY_ += response(filteredY_, yActive_) * deltaSeconds;

  const int x = constrain(static_cast<int>(lroundf(residualX_)), -127, 127);
  const int y = constrain(static_cast<int>(lroundf(residualY_)), -127, 127);
  residualX_ -= x;
  residualY_ -= y;

  dx = static_cast<int8_t>(x);
  dy = static_cast<int8_t>(y);
  return dx != 0 || dy != 0;
}

void CursorEngine::reset() {
  filteredX_ = 0.0F;
  filteredY_ = 0.0F;
  residualX_ = 0.0F;
  residualY_ = 0.0F;
  lastUpdateMs_ = 0;
  xActive_ = false;
  yActive_ = false;
}

float CursorEngine::response(float dps, bool& axisActive) {
  const float magnitude = fabsf(dps);
  if (axisActive) {
    if (magnitude <= kDeadZoneExitDps) {
      axisActive = false;
      return 0.0F;
    }
  } else if (magnitude < kDeadZoneEnterDps) {
    return 0.0F;
  } else {
    axisActive = true;
  }

  const float active = magnitude - kDeadZoneExitDps;
  const float pixels =
      min(kLinearPixelsPerSecondPerDps * active +
              kAccelerationPixelsPerSecondPerDps2 * active * active,
          kMaxPixelsPerSecond);
  return copysignf(pixels, dps);
}

}  // namespace input
