#pragma once

#include <Arduino.h>

class Arduino_DataBus;
class Arduino_GFX;

namespace drivers {

class DisplayDevice {
 public:
  DisplayDevice();

  bool begin();
  void setBrightness(uint8_t percent);
  Arduino_GFX& gfx();

 private:
  Arduino_DataBus* bus_;
  Arduino_GFX* gfx_;
};

}  // namespace drivers

