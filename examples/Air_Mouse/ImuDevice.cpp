#include "ImuDevice.h"

#include <cmath>

#include "BoardConfig.h"

namespace drivers {

ImuDevice::ImuDevice(TwoWire& wire)
    : I2cRegisterDevice(wire, board::kImuAddress) {}

bool ImuDevice::begin() {
  pinMode(board::kImuControl, OUTPUT);
#if TQT_C6_BATTERY_V12
  digitalWrite(board::kImuControl, HIGH);
#else
  digitalWrite(board::kImuControl, LOW);
#endif
  delay(20);

  uint8_t whoAmI = 0;
  if (!probe() || !readRegister(kWhoAmIReg, whoAmI) ||
      whoAmI != kExpectedWhoAmI) {
    healthy_ = false;
    return false;
  }

  // BDU + register auto-increment, then gyro at 104 Hz / 250 dps.
  healthy_ = writeRegister(kCtrl3CReg, 0x44) &&
             writeRegister(kCtrl2GReg, 0x40);
  delay(100);
  return healthy_;
}

bool ImuDevice::startStreaming() {
  if (!healthy_) {
    return false;
  }

  // Gyro-only FIFO avoids stale samples when the UI task is busy.
  fifoEnabled_ = writeRegister(kFifoCtrl5Reg, 0x00) &&
                 writeRegister(kFifoCtrl2Reg, 0x00) &&
                 writeRegister(kFifoCtrl3Reg, 0x08) &&
                 writeRegister(kFifoCtrl4Reg, 0x00) &&
                 writeRegister(kFifoCtrl5Reg, 0x26);
  return fifoEnabled_;
}

bool ImuDevice::calibrate(uint16_t sampleCount) {
  if (!healthy_ || sampleCount == 0) {
    return false;
  }

  double meanX = 0.0;
  double meanY = 0.0;
  double meanZ = 0.0;
  double m2X = 0.0;
  double m2Y = 0.0;
  double m2Z = 0.0;
  uint16_t accepted = 0;
  const uint32_t deadline = millis() + static_cast<uint32_t>(sampleCount) * 20U;

  while (accepted < sampleCount && static_cast<int32_t>(deadline - millis()) > 0) {
    uint8_t status = 0;
    if (!readRegister(kStatusReg, status) || (status & 0x02U) == 0) {
      delay(2);
      continue;
    }

    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    if (readRawGyro(x, y, z)) {
      ++accepted;

      const double dx = static_cast<double>(x) - meanX;
      meanX += dx / accepted;
      m2X += dx * (static_cast<double>(x) - meanX);

      const double dy = static_cast<double>(y) - meanY;
      meanY += dy / accepted;
      m2Y += dy * (static_cast<double>(y) - meanY);

      const double dz = static_cast<double>(z) - meanZ;
      meanZ += dz / accepted;
      m2Z += dz * (static_cast<double>(z) - meanZ);
    }
    delay(2);
  }

  if (accepted != sampleCount || accepted < 2) {
    return false;
  }

  const float stdDevX = sqrt(m2X / (accepted - 1)) * kDpsPerLsb;
  const float stdDevY = sqrt(m2Y / (accepted - 1)) * kDpsPerLsb;
  const float stdDevZ = sqrt(m2Z / (accepted - 1)) * kDpsPerLsb;
  if (stdDevX > kMaxCalibrationStdDevDps ||
      stdDevY > kMaxCalibrationStdDevDps ||
      stdDevZ > kMaxCalibrationStdDevDps) {
    return false;
  }

  biasX_ = static_cast<float>(meanX) * kDpsPerLsb;
  biasY_ = static_cast<float>(meanY) * kDpsPerLsb;
  biasZ_ = static_cast<float>(meanZ) * kDpsPerLsb;
  lastSampleMs_ = millis();
  return true;
}

bool ImuDevice::update(uint32_t nowMs) {
  if (!healthy_ || nowMs - lastSampleMs_ < kSampleIntervalMs) {
    return false;
  }
  lastSampleMs_ = nowMs;

  int16_t x = 0;
  int16_t y = 0;
  int16_t z = 0;
  if (fifoEnabled_) {
    uint8_t fifoStatus[2]{};
    if (!readRegisters(kFifoStatus1Reg, fifoStatus, sizeof(fifoStatus))) {
      return false;
    }
    uint16_t words = static_cast<uint16_t>(fifoStatus[0]) |
                     static_cast<uint16_t>(fifoStatus[1] & 0x03U) << 8;
    const uint16_t samples = words / 3U;
    if (samples == 0) {
      return false;
    }

    // Drain old samples and keep only the newest motion sample.
    uint8_t data[6]{};
    const uint16_t toRead = samples > 32U ? 32U : samples;
    for (uint16_t i = 0; i < toRead; ++i) {
      if (!readRegisters(kFifoDataReg, data, sizeof(data))) {
        return false;
      }
    }
    x = static_cast<int16_t>((static_cast<uint16_t>(data[1]) << 8) | data[0]);
    y = static_cast<int16_t>((static_cast<uint16_t>(data[3]) << 8) | data[2]);
    z = static_cast<int16_t>((static_cast<uint16_t>(data[5]) << 8) | data[4]);
  } else {
    uint8_t status = 0;
    if (!readRegister(kStatusReg, status) || (status & 0x02U) == 0 ||
        !readRawGyro(x, y, z)) {
      return false;
    }
  }

  sample_.xDps = x * kDpsPerLsb - biasX_;
  sample_.yDps = y * kDpsPerLsb - biasY_;
  sample_.zDps = z * kDpsPerLsb - biasZ_;
  return true;
}

const GyroSample& ImuDevice::gyro() const { return sample_; }

bool ImuDevice::healthy() const { return healthy_; }

bool ImuDevice::readRawGyro(int16_t& x, int16_t& y, int16_t& z) {
  uint8_t data[6]{};
  if (!readRegisters(kOutXLowGyroReg, data, sizeof(data))) {
    return false;
  }

  x = static_cast<int16_t>((static_cast<uint16_t>(data[1]) << 8) | data[0]);
  y = static_cast<int16_t>((static_cast<uint16_t>(data[3]) << 8) | data[2]);
  z = static_cast<int16_t>((static_cast<uint16_t>(data[5]) << 8) | data[4]);
  return true;
}

}  // namespace drivers
