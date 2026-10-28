#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace drivers {

class PowerManager {
 public:
  explicit PowerManager(TwoWire& wire);

  bool begin();
  bool startMonitoring();
  bool charging() const;
  uint16_t batteryRaw() const;
  uint8_t batteryPercent() const;

 private:
  bool writeRegister(uint8_t reg, uint8_t value) const;
  bool readRegister(uint8_t reg, uint8_t& value) const;
  bool initializeSgm41562() const;
  bool readCharging() const;
  void sample();
  static void monitorTaskEntry(void* context);
  void monitorTaskLoop();
  uint16_t readBatteryRaw() const;
  uint16_t readBatteryMillivolts() const;
  TwoWire& wire_;
  bool present_{false};
  TaskHandle_t monitorTask_{nullptr};  // ADC and PMU sampling worker.
  std::atomic<bool> charging_{false};
  std::atomic<uint16_t> batteryRaw_{0};
  std::atomic<uint8_t> batteryPercent_{0};
};

}  // namespace drivers
