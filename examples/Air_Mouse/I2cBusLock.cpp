#include "I2cBusLock.h"

namespace drivers {

SemaphoreHandle_t I2cBusLock::mutex_ = nullptr;

I2cBusLock::I2cBusLock() {
  if (mutex_ == nullptr) {
    mutex_ = xSemaphoreCreateMutex();
  }
  locked_ = mutex_ != nullptr &&
            xSemaphoreTake(mutex_, pdMS_TO_TICKS(20)) == pdTRUE;
}

I2cBusLock::~I2cBusLock() {
  if (locked_) {
    xSemaphoreGive(mutex_);
  }
}

}  // namespace drivers
