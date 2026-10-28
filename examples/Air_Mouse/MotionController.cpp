#include "MotionController.h"

namespace input {

MotionController::MotionController(drivers::ImuDevice& imu, CursorEngine& cursor,
                                   ble::BleMouseService& mouse)
    : imu_(imu), cursor_(cursor), mouse_(mouse) {}

MotionController::~MotionController() {
  if (task_ != nullptr) {
    vTaskDelete(task_);
    task_ = nullptr;
  }
}

bool MotionController::begin() {
  return xTaskCreate(taskEntry, "imu_motion", 4096, this, 1, &task_) == pdPASS;
}

void MotionController::setActive(bool active) {
  active_.store(active);
}

void MotionController::freezeUntil(uint32_t deadlineMs) {
  freezeUntilMs_.store(deadlineMs);
}

void MotionController::taskEntry(void* context) {
  static_cast<MotionController*>(context)->taskLoop();
}

void MotionController::taskLoop() {
  TickType_t wake = xTaskGetTickCount();
  bool wasActive = false;
  for (;;) {
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(4));
    const bool active = active_.load();
    if (!active || !imu_.healthy()) {
      if (wasActive) {
        cursor_.reset();
        wasActive = false;
      }
      continue;
    }
    if (!wasActive) {
      cursor_.reset();
      wasActive = true;
    }

    const uint32_t nowMs = millis();
    if (static_cast<int32_t>(nowMs - freezeUntilMs_.load()) < 0) {
      continue;
    }

    if (!imu_.update(nowMs)) {
      continue;
    }

    int8_t dx = 0;
    int8_t dy = 0;
    if (cursor_.update(imu_.gyro(), nowMs, dx, dy)) {
      mouse_.move(dx, dy);
    }
  }
}

}  // namespace input
