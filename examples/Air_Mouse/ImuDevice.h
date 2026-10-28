#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "I2cRegisterDevice.h"

namespace drivers {

struct GyroSample {
  float xDps{0.0F};
  float yDps{0.0F};
  float zDps{0.0F};
};

class ImuDevice : private I2cRegisterDevice {
 public:
  explicit ImuDevice(TwoWire& wire);

  bool begin();
  bool calibrate(uint16_t sampleCount = 160);
  bool startStreaming();
  bool update(uint32_t nowMs);
  const GyroSample& gyro() const;
  bool healthy() const;

 private:
  bool readRawGyro(int16_t& x, int16_t& y, int16_t& z);

  static constexpr uint8_t kWhoAmIReg = 0x0F;
  static constexpr uint8_t kExpectedWhoAmI = 0x6A;
  static constexpr uint8_t kCtrl2GReg = 0x11;
  static constexpr uint8_t kCtrl3CReg = 0x12;
  static constexpr uint8_t kStatusReg = 0x1E;
  static constexpr uint8_t kFifoCtrl2Reg = 0x07;
  static constexpr uint8_t kFifoCtrl3Reg = 0x08;
  static constexpr uint8_t kFifoCtrl4Reg = 0x09;
  static constexpr uint8_t kFifoCtrl5Reg = 0x0A;
  static constexpr uint8_t kFifoStatus1Reg = 0x3A;
  static constexpr uint8_t kFifoDataReg = 0x3E;
  static constexpr uint8_t kOutXLowGyroReg = 0x22;
  static constexpr float kDpsPerLsb = 0.00875F;
  static constexpr float kMaxCalibrationStdDevDps = 0.2F;
  static constexpr uint32_t kSampleIntervalMs = 8;

  GyroSample sample_{};
  float biasX_{0.0F};
  float biasY_{0.0F};
  float biasZ_{0.0F};
  bool healthy_{false};
  bool fifoEnabled_{false};
  uint32_t lastSampleMs_{0};
};

}  // namespace drivers
