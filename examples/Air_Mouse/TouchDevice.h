#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "I2cRegisterDevice.h"

namespace drivers {

struct TouchPoint {
  bool down{false};
  uint16_t x{0};
  uint16_t y{0};
};

class TouchDevice : private I2cRegisterDevice {
 public:
  explicit TouchDevice(TwoWire& wire);

  bool begin();
  void update(uint32_t nowMs);
  const TouchPoint& point() const;

 private:
  static constexpr uint8_t kFingerCountReg = 0x02;
  static constexpr uint8_t kChipIdReg = 0xA7;
  static constexpr uint32_t kPollIntervalMs = 8;
  static constexpr uint32_t kReadFailureReleaseMs = 80;

  TouchPoint point_{};
  bool healthy_{false};
  uint32_t lastPollMs_{0};
  uint32_t lastValidTouchMs_{0};
};

}  // namespace drivers
