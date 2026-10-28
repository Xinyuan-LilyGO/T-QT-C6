#include "UiController.h"

#include <cstdio>

#include "BoardConfig.h"

namespace ui {
namespace {

const lv_color_t kBackground = LV_COLOR_MAKE(18, 20, 24);
const lv_color_t kPanel = LV_COLOR_MAKE(43, 47, 54);
const lv_color_t kLeftAccent = LV_COLOR_MAKE(40, 167, 121);
const lv_color_t kRightAccent = LV_COLOR_MAKE(230, 163, 52);
const lv_color_t kPressed = LV_COLOR_MAKE(240, 244, 248);
const lv_color_t kText = LV_COLOR_MAKE(245, 247, 250);
const lv_color_t kMuted = LV_COLOR_MAKE(158, 166, 178);

void prepareScreen(lv_obj_t* screen) {
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(screen, kBackground, 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
}

}  // namespace

void UiController::begin() {
  pairingScreen_ = lv_obj_create(nullptr);
  prepareScreen(pairingScreen_);

  pairingLabel_ = lv_label_create(pairingScreen_);
  lv_obj_set_style_text_color(pairingLabel_, kText, 0);
  lv_obj_set_style_text_font(pairingLabel_, &lv_font_montserrat_14, 0);
  lv_label_set_text(pairingLabel_, "PAIRING...");
  lv_obj_align(pairingLabel_, LV_ALIGN_CENTER, 0, -8);

  pairingDetail_ = lv_label_create(pairingScreen_);
  lv_obj_set_style_text_color(pairingDetail_, kMuted, 0);
  lv_obj_set_style_text_font(pairingDetail_, &lv_font_montserrat_12, 0);
  lv_label_set_text(pairingDetail_, "TQT Air Mouse");
  lv_obj_align(pairingDetail_, LV_ALIGN_CENTER, 0, 16);

  controlScreen_ = lv_obj_create(nullptr);
  prepareScreen(controlScreen_);

  statusLabel_ = lv_label_create(controlScreen_);
  lv_obj_set_width(statusLabel_, board::kScreenWidth);
  lv_obj_set_style_text_align(statusLabel_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(statusLabel_, kMuted, 0);
  lv_obj_set_style_text_font(statusLabel_, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(statusLabel_, 0, 3);
  lv_label_set_text(statusLabel_, "READY");

  batteryLabel_ = lv_label_create(controlScreen_);
  lv_obj_set_style_text_font(batteryLabel_, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(batteryLabel_, kText, 0);
  lv_obj_set_pos(batteryLabel_, 3, 3);
  lv_label_set_text(batteryLabel_, "--%");

  leftPanel_ = createButtonPanel(controlScreen_, 2, kLeftAccent, "L", "LEFT");
  rightPanel_ =
      createButtonPanel(controlScreen_, 65, kRightAccent, "R", "RIGHT");

  lv_scr_load(pairingScreen_);
}

void UiController::showPairing(bool linkConnected) {
  // Keep the same pairing screen during host reconnect and authentication.
  lv_label_set_text(pairingLabel_, "PAIRING...");
  lv_label_set_text(pairingDetail_, linkConnected ? "CONNECTING..."
                                                 : "TQT Air Mouse");
  lv_scr_load(pairingScreen_);
}

void UiController::showCalibrating() {
  lv_label_set_text(pairingLabel_, "CALIBRATING");
  lv_label_set_text(pairingDetail_, "KEEP STILL");
  lv_scr_load(pairingScreen_);
}

void UiController::showControl(bool imuOk, bool touchOk) {
  if (!imuOk) {
    lv_label_set_text(statusLabel_, "IMU ERROR");
  } else if (!touchOk) {
    lv_label_set_text(statusLabel_, "TOUCH ERROR");
  } else {
    lv_label_set_text(statusLabel_, "CONNECTED");
  }
  lv_scr_load(controlScreen_);
}

void UiController::setBattery(uint8_t percent, bool charging) {
  if (batteryLabel_ == nullptr) {
    return;
  }
  char text[8]{};
  snprintf(text, sizeof(text), "%u%%", percent > 100 ? 100 : percent);
  lv_label_set_text(batteryLabel_, text);
  lv_obj_set_style_text_color(batteryLabel_, charging ? kLeftAccent : kText,
                              0);
}

void UiController::setButtonPressed(input::MouseButton button, bool pressed) {
  lv_obj_t* panel = nullptr;
  lv_color_t normalColor = kPanel;
  if (button == input::MouseButton::Left) {
    panel = leftPanel_;
  } else if (button == input::MouseButton::Right) {
    panel = rightPanel_;
  }
  if (panel == nullptr) {
    return;
  }

  lv_obj_set_style_bg_color(panel, pressed ? kPressed : normalColor, 0);
  lv_obj_set_style_text_color(panel, pressed ? kBackground : kText, 0);
}

lv_obj_t* UiController::createButtonPanel(lv_obj_t* parent, lv_coord_t x,
                                          lv_color_t color, const char* title,
                                          const char* subtitle) {
  lv_obj_t* panel = lv_obj_create(parent);
  lv_obj_remove_style_all(panel);
  lv_obj_set_pos(panel, x, 20);
  lv_obj_set_size(panel, 61, 106);
  lv_obj_set_style_radius(panel, 4, 0);
  lv_obj_set_style_bg_color(panel, kPanel, 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(panel, color, 0);
  lv_obj_set_style_border_width(panel, 2, 0);
  lv_obj_set_style_text_color(panel, kText, 0);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* titleLabel = lv_label_create(panel);
  lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
  lv_label_set_text(titleLabel, title);
  lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, -10);

  lv_obj_t* subtitleLabel = lv_label_create(panel);
  lv_obj_set_style_text_font(subtitleLabel, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitleLabel, color, 0);
  lv_label_set_text(subtitleLabel, subtitle);
  lv_obj_align(subtitleLabel, LV_ALIGN_CENTER, 0, 14);
  return panel;
}

}  // namespace ui
