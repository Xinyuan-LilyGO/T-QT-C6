#pragma once

#include <Arduino.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "BleMouseService.h"
#include "CursorEngine.h"
#include "ImuDevice.h"

namespace input {

// Samples the IMU away from LVGL, touch, and battery work.
class MotionController {
 public:
  MotionController(drivers::ImuDevice& imu, CursorEngine& cursor,
                   ble::BleMouseService& mouse);
  ~MotionController();

  bool begin();
  void setActive(bool active);
  void freezeUntil(uint32_t deadlineMs);

 private:
  static void taskEntry(void* context);
  void taskLoop();

  drivers::ImuDevice& imu_;
  CursorEngine& cursor_;
  ble::BleMouseService& mouse_;
  TaskHandle_t task_{nullptr};
  std::atomic<bool> active_{false};
  std::atomic<uint32_t> freezeUntilMs_{0};
};

}  // namespace input
