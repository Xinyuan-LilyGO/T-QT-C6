#pragma once

#include <Arduino.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class NimBLECharacteristic;
class NimBLEConnInfo;
class NimBLEHIDDevice;
class NimBLEServer;

namespace ble {

class BleMouseService {
 public:
  BleMouseService();
  ~BleMouseService();

  bool begin(const char* deviceName);
  bool ready() const;
  bool linkConnected() const;
  void forceReconnect();

  void move(int8_t dx, int8_t dy, int8_t wheel = 0);
  void pressLeft();
  void releaseLeft();
  void pressRight();
  void releaseRight();
  void releaseAll();

 private:
  class ServerCallbacks;

  bool sendReport(uint8_t buttons, int8_t dx, int8_t dy, int8_t wheel);
  void setButton(uint8_t mask, bool pressed);
  void updateReportInterval(const NimBLEConnInfo& connection);
  static void reportTaskEntry(void* context);
  void reportTaskLoop();
  void wakeReportTask();
  bool takePending(int8_t& dx, int8_t& dy, int8_t& wheel, uint8_t& buttons,
                   bool& hasButton, bool& force);
  void restorePending(int8_t dx, int8_t dy, int8_t wheel, uint8_t buttons,
                      bool hasButton, bool force);
  void enqueueButtonState(uint8_t buttons, bool front = false);

  NimBLEServer* server_{nullptr};
  NimBLEHIDDevice* hid_{nullptr};
  NimBLECharacteristic* inputReport_{nullptr};
  ServerCallbacks* callbacks_{nullptr};
  std::atomic<bool> linkConnected_{false};
  std::atomic<bool> ready_{false};
  std::atomic<uint16_t> connectionHandle_{0xFFFF};
  std::atomic<uint8_t> buttons_{0};
  std::atomic<uint16_t> reportIntervalMs_{15};
  TaskHandle_t reportTask_{nullptr};
  portMUX_TYPE pendingMux_ = portMUX_INITIALIZER_UNLOCKED;
  int16_t pendingX_{0};
  int16_t pendingY_{0};
  int16_t pendingWheel_{0};
  uint8_t buttonQueue_[8]{};  // Preserve press/release edges across retries.
  uint8_t buttonQueueSize_{0};
  uint32_t lastReportMs_{0};
  std::atomic<uint16_t> consecutiveNotifyFailures_{0};
  bool reportDirty_{false};
  bool forceReport_{false};
};

}  // namespace ble
