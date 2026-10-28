#include "PowerManager.h"

#include "BoardConfig.h"
#include "I2cBusLock.h"

namespace drivers {
namespace {

constexpr uint8_t kSgm41562Address = 0x03;
constexpr uint8_t kSgm41562DeviceIdRegister = 0x0B;
constexpr uint8_t kSgm41562ExpectedDeviceId = 0x04;
constexpr uint8_t kSgm41562StatusRegister = 0x08;

}  // namespace

PowerManager::PowerManager(TwoWire& wire) : wire_(wire) {}

bool PowerManager::begin() {
  pinMode(board::kPmuInterrupt, INPUT_PULLUP);

  // Battery V1.2 enables the bottom-module rail through GPIO3. This must be
  // done before probing or initializing the SGM41562, as in LilyGO's example.
#if TQT_C6_BATTERY_V12
  pinMode(board::kImuControl, OUTPUT);
  digitalWrite(board::kImuControl, HIGH);
  delay(20);
#endif

  pinMode(board::kBatteryMeasurementControl, OUTPUT);
  // GPIO8 is active-low: LOW enables the divider, HIGH disconnects it.
  digitalWrite(board::kBatteryMeasurementControl, HIGH);
  pinMode(board::kBatteryAdc, INPUT_PULLDOWN);
  analogReadResolution(12);

#if TQT_C6_BATTERY_V12
  // Use the already initialized Arduino Wire bus. cpp_bus_driver's ESP-IDF
  // backends cannot be mixed with Arduino-ESP32's I2C driver on this chip.
  present_ = initializeSgm41562();
  uint8_t deviceId = 0;
  if (!present_ || !readRegister(kSgm41562DeviceIdRegister, deviceId) ||
      deviceId != kSgm41562ExpectedDeviceId) {
    present_ = false;
  }
  Serial0.printf("PMU: SGM41562 addr=0x%02X Wire init=%s id=0x%02X\n",
                 kSgm41562Address, present_ ? "OK" : "FAILED", deviceId);
#else
  present_ = false;
  Serial0.println("PMU: this build is configured for Battery V1.2");
#endif
  if (present_) {
    sample();
  }
  return present_;
}

bool PowerManager::startMonitoring() {
  if (!present_) {
    return false;
  }
  if (monitorTask_ != nullptr) {
    return true;
  }
  return xTaskCreate(monitorTaskEntry, "power_monitor", 3072, this, 1,
                     &monitorTask_) == pdPASS;
}

bool PowerManager::charging() const { return charging_.load(); }

bool PowerManager::readCharging() const {
#if TQT_C6_BATTERY_V12
  if (!present_) {
    return false;
  }
  uint8_t status = 0;
  if (!readRegister(kSgm41562StatusRegister, status)) {
    return false;
  }
  const uint8_t chargeStatus = (status >> 3) & 0x03U;
  return chargeStatus == 1U || chargeStatus == 2U;
#else
  return false;
#endif
}

void PowerManager::sample() {
  charging_.store(readCharging());
  batteryRaw_.store(readBatteryRaw());
  const uint32_t millivolts = readBatteryMillivolts();
  uint8_t percent = 0;
  if (millivolts >= 4200U) {
    percent = 100;
  } else if (millivolts > 3300U) {
    percent = static_cast<uint8_t>((millivolts - 3300U) * 100U / 900U);
  }
  batteryPercent_.store(percent);
}

void PowerManager::monitorTaskEntry(void* context) {
  static_cast<PowerManager*>(context)->monitorTaskLoop();
}

void PowerManager::monitorTaskLoop() {
  TickType_t wake = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(1000));
    sample();
  }
}

bool PowerManager::writeRegister(uint8_t reg, uint8_t value) const {
  I2cBusLock lock;
  if (!lock.locked()) {
    return false;
  }
  wire_.beginTransmission(kSgm41562Address);
  wire_.write(reg);
  wire_.write(value);
  return wire_.endTransmission() == 0;
}

bool PowerManager::readRegister(uint8_t reg, uint8_t& value) const {
  I2cBusLock lock;
  if (!lock.locked()) {
    return false;
  }
  wire_.beginTransmission(kSgm41562Address);
  wire_.write(reg);
  if (wire_.endTransmission(false) != 0 ||
      wire_.requestFrom(kSgm41562Address, static_cast<uint8_t>(1), true) != 1 ||
      !wire_.available()) {
    return false;
  }
  value = static_cast<uint8_t>(wire_.read());
  return true;
}

bool PowerManager::initializeSgm41562() const {
  // Same functional sequence as cpp_bus_driver's Sgm41562xx::Init(). The
  // V1.2 two-wire battery board has no NTC thermistor, so NTC is disabled.
  delay(120);
  return writeRegister(0x07, 0xB7) &&  // system voltage / OTP
         writeRegister(0x06, 0x40) &&  // NTC disabled
         writeRegister(0x05, 0x1A) &&  // watchdog disabled
         writeRegister(0x01, 0xA4) &&  // charge enabled
         writeRegister(0x08, 0x40);    // input current limit disabled
}

uint16_t PowerManager::readBatteryRaw() const {
  digitalWrite(board::kBatteryMeasurementControl, LOW);
  delay(2);
  uint32_t total = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    total += static_cast<uint32_t>(analogRead(board::kBatteryAdc));
    delayMicroseconds(250);
  }
  digitalWrite(board::kBatteryMeasurementControl, HIGH);
  return static_cast<uint16_t>(total / 4U);
}

uint16_t PowerManager::readBatteryMillivolts() const {
  digitalWrite(board::kBatteryMeasurementControl, LOW);
  delay(2);
  uint32_t total = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    total += static_cast<uint32_t>(analogReadMilliVolts(board::kBatteryAdc));
    delayMicroseconds(250);
  }
  digitalWrite(board::kBatteryMeasurementControl, HIGH);
  return static_cast<uint16_t>((total / 4U) * 2U);
}

uint16_t PowerManager::batteryRaw() const {
  return batteryRaw_.load();
}

uint8_t PowerManager::batteryPercent() const {
  return batteryPercent_.load();
}

}  // namespace drivers
