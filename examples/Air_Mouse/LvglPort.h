#pragma once

#include <Arduino.h>
#include <lvgl.h>

#include "BoardConfig.h"

namespace drivers {
class DisplayDevice;
}

namespace ui {

class LvglPort {
 public:
  explicit LvglPort(drivers::DisplayDevice& display);

  bool begin();
  void task(uint32_t nowMs, uint32_t handlerIntervalMs = 5);

 private:
  static void flush(lv_disp_drv_t* driver, const lv_area_t* area,
                    lv_color_t* colors);

  static LvglPort* active_;
  static constexpr uint16_t kBufferLines = 24;

  drivers::DisplayDevice& display_;
  lv_disp_draw_buf_t drawBufferDescriptor_{};
  lv_disp_drv_t displayDriver_{};
  lv_color_t drawBuffer_[board::kScreenWidth * kBufferLines]{};
  uint32_t lastTickMs_{0};
  uint32_t lastHandlerMs_{0};
};

}  // namespace ui
