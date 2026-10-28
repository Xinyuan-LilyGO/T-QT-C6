#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace drivers {

// One mutex protects register-pointer plus read transactions on the shared bus.
class I2cBusLock {
 public:
  I2cBusLock();
  ~I2cBusLock();
  bool locked() const { return locked_; }

 private:
  static SemaphoreHandle_t mutex_;
  bool locked_{false};
};

}  // namespace drivers
