#include "LvglPort.h"

#include <Arduino_GFX_Library.h>

#include "DisplayDevice.h"

namespace ui {

LvglPort* LvglPort::active_ = nullptr;

LvglPort::LvglPort(drivers::DisplayDevice& display) : display_(display) {}

bool LvglPort::begin() {
  active_ = this;
  lv_init();

  lv_disp_draw_buf_init(&drawBufferDescriptor_, drawBuffer_, nullptr,
                        board::kScreenWidth * kBufferLines);
  lv_disp_drv_init(&displayDriver_);
  displayDriver_.hor_res = board::kScreenWidth;
  displayDriver_.ver_res = board::kScreenHeight;
  displayDriver_.flush_cb = flush;
  displayDriver_.draw_buf = &drawBufferDescriptor_;

  if (lv_disp_drv_register(&displayDriver_) == nullptr) {
    return false;
  }

  lastTickMs_ = millis();
  lastHandlerMs_ = lastTickMs_;
  return true;
}

void LvglPort::task(uint32_t nowMs, uint32_t handlerIntervalMs) {
  const uint32_t elapsed = nowMs - lastTickMs_;
  if (elapsed > 0) {
    lv_tick_inc(elapsed);
    lastTickMs_ = nowMs;
  }

  if (nowMs - lastHandlerMs_ >= handlerIntervalMs) {
    lv_timer_handler();
    lastHandlerMs_ = nowMs;
  }
}

void LvglPort::flush(lv_disp_drv_t* driver, const lv_area_t* area,
                     lv_color_t* colors) {
  if (active_ == nullptr) {
    lv_disp_flush_ready(driver);
    return;
  }

  const uint16_t width = static_cast<uint16_t>(area->x2 - area->x1 + 1);
  const uint16_t height = static_cast<uint16_t>(area->y2 - area->y1 + 1);
#if LV_COLOR_16_SWAP
  active_->display_.gfx().draw16bitBeRGBBitmap(
      area->x1, area->y1, reinterpret_cast<uint16_t*>(&colors->full), width,
      height);
#else
  active_->display_.gfx().draw16bitRGBBitmap(
      area->x1, area->y1, reinterpret_cast<uint16_t*>(&colors->full), width,
      height);
#endif
  lv_disp_flush_ready(driver);
}

}  // namespace ui
