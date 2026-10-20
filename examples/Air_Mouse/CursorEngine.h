#pragma once

#include <Arduino.h>

#include "ImuDevice.h"

namespace input {

class CursorEngine {
 public:
  bool update(const drivers::GyroSample& gyro, uint32_t nowMs, int8_t& dx,
              int8_t& dy);
  void reset();

 private:
  static float response(float dps, bool& axisActive);

  static constexpr float kFilterTimeConstantSeconds = 0.025F;
  static constexpr float kDeadZoneEnterDps = 0.8F;
  static constexpr float kDeadZoneExitDps = 0.45F;
  static constexpr float kLinearPixelsPerSecondPerDps = 7.28F;
  static constexpr float kAccelerationPixelsPerSecondPerDps2 = 0.208F;
  static constexpr float kMaxPixelsPerSecond = 2600.0F;
  static constexpr float kMaxDeltaSeconds = 0.025F;

  float filteredX_{0.0F};
  float filteredY_{0.0F};
  float residualX_{0.0F};
  float residualY_{0.0F};
  uint32_t lastUpdateMs_{0};
  bool xActive_{false};
  bool yActive_{false};
};

}  // namespace input
