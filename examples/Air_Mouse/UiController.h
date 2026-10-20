#pragma once

#include <lvgl.h>

#include "TouchButtons.h"

namespace ui {

class UiController {
 public:
  void begin();
  void showCalibrating();
  void showPairing(bool linkConnected);
  void showControl(bool imuOk, bool touchOk);
  void setBattery(uint8_t percent, bool charging);
  void setButtonPressed(input::MouseButton button, bool pressed);

 private:
  static lv_obj_t* createButtonPanel(lv_obj_t* parent, lv_coord_t x,
                                     lv_color_t color, const char* title,
                                     const char* subtitle);

  lv_obj_t* pairingScreen_{nullptr};
  lv_obj_t* pairingLabel_{nullptr};
  lv_obj_t* pairingDetail_{nullptr};
  lv_obj_t* controlScreen_{nullptr};
  lv_obj_t* statusLabel_{nullptr};
  lv_obj_t* batteryLabel_{nullptr};
  lv_obj_t* leftPanel_{nullptr};
  lv_obj_t* rightPanel_{nullptr};
};

}  // namespace ui
