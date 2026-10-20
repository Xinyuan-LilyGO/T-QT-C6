#include "DisplayDevice.h"

#include <Arduino_GFX_Library.h>

#include "BoardConfig.h"

namespace drivers {

DisplayDevice::DisplayDevice()
    : bus_(new Arduino_HWSPI(board::kLcdDc, board::kLcdCs, board::kLcdSclk,
                             board::kLcdMosi, board::kLcdMiso)),
      gfx_(nullptr) {
#if TQT_C6_LCD_DRIVER == 1
  gfx_ = new Arduino_GC9107(bus_, board::kLcdReset, 0, true,
                            board::kScreenWidth, board::kScreenHeight,
                            board::kDisplayRotation, 1, 0, 0);
#elif TQT_C6_LCD_DRIVER == 2
  gfx_ = new Arduino_ST7735(bus_, board::kLcdReset, 0, true,
                            board::kScreenWidth, board::kScreenHeight,
                            board::kDisplayRotation, 3, 2, 3);
#else
#error "TQT_C6_LCD_DRIVER must be 1 (GC9107) or 2 (ST7735)"
#endif
}

bool DisplayDevice::begin() {
  pinMode(board::kLcdBacklight, OUTPUT);
  if (!ledcAttach(board::kLcdBacklight, 20000, 8)) {
    return false;
  }
  ledcWrite(board::kLcdBacklight, board::kBacklightOffDuty);

  if (!gfx_->begin()) {
    return false;
  }
  gfx_->fillScreen(RGB565_BLACK);
  setBrightness(100);
  return true;
}

void DisplayDevice::setBrightness(uint8_t percent) {
  percent = constrain(percent, 0, 100);
  const uint8_t duty = static_cast<uint8_t>(
      map(percent, 0, 100, board::kBacklightOffDuty, board::kBacklightOnDuty));
  ledcWrite(board::kLcdBacklight, duty);
}

Arduino_GFX& DisplayDevice::gfx() { return *gfx_; }

}  // namespace drivers
