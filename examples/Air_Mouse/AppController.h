#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "BleMouseService.h"
#include "DisplayDevice.h"
#include "ImuDevice.h"
#include "PowerManager.h"
#include "TouchDevice.h"
#include "CursorEngine.h"
#include "TouchButtons.h"
#include "LvglPort.h"
#include "UiController.h"
#include "MotionController.h"

namespace app {

class AppController {
 public:
  AppController();

  bool begin();
  void loop();

 private:
  void handleConnectionState(bool ready, bool linkConnected);
  void handleButtonEvent(const input::ButtonEvent& event, uint32_t nowMs);
  void updateBatteryUi(uint32_t nowMs);

  drivers::DisplayDevice display_;
  drivers::TouchDevice touch_;
  drivers::ImuDevice imu_;
  drivers::PowerManager power_;
  ble::BleMouseService mouse_;
  input::TouchButtons touchButtons_;
  input::CursorEngine cursor_;
  input::MotionController motion_;
  ui::LvglPort lvgl_;
  ui::UiController ui_;

  bool touchOk_{false};
  bool imuOk_{false};
  bool lastReady_{false};
  bool lastLinkConnected_{false};
  uint32_t notReadySinceMs_{0};
  bool reconnectRequested_{false};
  uint32_t lastBatteryUiMs_{0};
};

}  // namespace app
