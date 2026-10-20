#include "BleMouseService.h"

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEUtils.h>

namespace ble {
namespace {

constexpr uint8_t kLeftButton = 0x01;
constexpr uint8_t kRightButton = 0x02;
constexpr uint8_t kReportId = 0x01;

uint8_t kMouseReportMap[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, kReportId,   // Report ID
    0x09, 0x01,        // Usage (Pointer)
    0xA1, 0x00,        // Collection (Physical)
    0x05, 0x09,        // Usage Page (Buttons)
    0x19, 0x01,        // Usage Minimum (Button 1)
    0x29, 0x03,        // Usage Maximum (Button 3)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x95, 0x03,        // Report Count (3)
    0x75, 0x01,        // Report Size (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)
    0x95, 0x01,        // Report Count (1)
    0x75, 0x05,        // Report Size (5)
    0x81, 0x03,        // Input (Constant)
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x30,        // Usage (X)
    0x09, 0x31,        // Usage (Y)
    0x09, 0x38,        // Usage (Wheel)
    0x15, 0x81,        // Logical Minimum (-127)
    0x25, 0x7F,        // Logical Maximum (127)
    0x75, 0x08,        // Report Size (8)
    0x95, 0x03,        // Report Count (3)
    0x81, 0x06,        // Input (Data, Variable, Relative)
    0xC0,              // End Collection
    0xC0               // End Collection
};

struct __attribute__((packed)) MouseReport {
  uint8_t buttons;
  int8_t x;
  int8_t y;
  int8_t wheel;
};

int16_t saturatedAdd(int16_t current, int8_t delta) {
  const int32_t sum = static_cast<int32_t>(current) + delta;
  return static_cast<int16_t>(constrain(sum, -1024, 1024));
}

int8_t reportDelta(int16_t pending) {
  return static_cast<int8_t>(constrain(pending, -127, 127));
}

}  // namespace

class BleMouseService::ServerCallbacks : public NimBLEServerCallbacks {
 public:
  explicit ServerCallbacks(BleMouseService& owner) : owner_(owner) {}

  void onConnect(NimBLEServer* server, NimBLEConnInfo& connection) override {
    (void)server;
    owner_.linkConnected_.store(true);
    owner_.ready_.store(false);
    owner_.connectionHandle_.store(connection.getConnHandle());
    owner_.updateReportInterval(connection);
    Serial0.printf("BLE connected: interval=%u (%.2f ms), latency=%u, "
                  "timeout=%u (10 ms units)\n",
                  connection.getConnInterval(),
                  connection.getConnInterval() * 1.25F,
                  connection.getConnLatency(), connection.getConnTimeout());

    // A bonded host may already be encrypted when the callback runs.
    if (connection.isEncrypted()) {
      owner_.ready_.store(true);
    }

    int securityRc = 0;
    const bool securityStarted =
        NimBLEDevice::startSecurity(connection.getConnHandle(), &securityRc);
    Serial0.printf("BLE security start: %s rc=%d (%s)\n",
                   securityStarted ? "OK" : "FAILED", securityRc,
                   NimBLEUtils::returnCodeToString(securityRc));
  }

  void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int reason) override {
    owner_.buttons_.store(0);
    owner_.ready_.store(false);
    owner_.linkConnected_.store(false);
    owner_.connectionHandle_.store(0xFFFF);
    portENTER_CRITICAL(&owner_.pendingMux_);
    owner_.pendingX_ = 0;
    owner_.pendingY_ = 0;
    owner_.pendingWheel_ = 0;
    owner_.buttonQueueSize_ = 0;
    owner_.reportDirty_ = false;
    owner_.forceReport_ = false;
    portEXIT_CRITICAL(&owner_.pendingMux_);
    owner_.consecutiveNotifyFailures_.store(0);
    const char* reasonText = NimBLEUtils::returnCodeToString(reason);
    if (reason == 0x208 || (reason & 0xFF) == 0x08) {
      reasonText = "connection timeout";
    }
    Serial0.printf("BLE disconnected: reason=0x%X (%s)\n", reason,
                   reasonText);
    NimBLEDevice::startAdvertising();
  }

  void onAuthenticationComplete(NimBLEConnInfo& connection) override {
    const bool encrypted = connection.isEncrypted();
    owner_.ready_.store(encrypted);
    Serial0.printf("BLE authentication: %s\n",
                  encrypted ? "encrypted" : "failed");
    if (!encrypted) {
      NimBLEDevice::getServer()->disconnect(connection.getConnHandle());
      return;
    }
    // Let Windows/macOS keep their preferred parameters. Forced updates can
    // race host security and shorten the effective supervision timeout.
  }

  void onConnParamsUpdate(NimBLEConnInfo& connection) override {
    owner_.updateReportInterval(connection);
    Serial0.printf("BLE params: interval=%u (%.2f ms), latency=%u, "
                  "timeout=%u (10 ms units)\n",
                  connection.getConnInterval(),
                  connection.getConnInterval() * 1.25F,
                  connection.getConnLatency(), connection.getConnTimeout());
  }

 private:
  BleMouseService& owner_;
};

BleMouseService::BleMouseService() = default;

BleMouseService::~BleMouseService() {
  if (reportTask_ != nullptr) {
    vTaskDelete(reportTask_);
    reportTask_ = nullptr;
  }
  delete callbacks_;
}

bool BleMouseService::begin(const char* deviceName) {
  NimBLEDevice::init(deviceName);
  // Keep the radio on the robust 1M PHY and use a moderate board-safe power
  // level. Coded PHY is longer range but has much higher HID latency.
  NimBLEDevice::setPower(8);
  NimBLEDevice::setDefaultPhy(BLE_GAP_LE_PHY_1M_MASK,
                              BLE_GAP_LE_PHY_1M_MASK);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
  NimBLEDevice::setSecurityAuth(true, false, true);

  server_ = NimBLEDevice::createServer();
  if (server_ == nullptr) {
    return false;
  }

  callbacks_ = new ServerCallbacks(*this);
  server_->setCallbacks(callbacks_);

  hid_ = new NimBLEHIDDevice(server_);
  hid_->setManufacturer("LILYGO");
  hid_->setPnp(0x02, 0x303A, 0x4001, 0x0100);
  hid_->setHidInfo(0x00, 0x01);
  hid_->setReportMap(kMouseReportMap, sizeof(kMouseReportMap));
  hid_->setBatteryLevel(100);
  inputReport_ = hid_->getInputReport(kReportId);

  hid_->startServices();

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->setName(deviceName);
  advertising->setAppearance(HID_MOUSE);
  advertising->addServiceUUID(hid_->getHidService()->getUUID());
  // Advertise a moderate interval without forcing a post-pairing update.
  advertising->setPreferredParams(16, 32);  // 20-40 ms in 1.25 ms units.
  advertising->enableScanResponse(true);
  if (!advertising->start()) {
    return false;
  }

  // Keep notify timing independent from the Arduino loop and LVGL flushes.
  return xTaskCreate(reportTaskEntry, "ble_report", 4096, this, 2,
                     &reportTask_) == pdPASS;
}

bool BleMouseService::ready() const { return ready_.load(); }

bool BleMouseService::linkConnected() const { return linkConnected_.load(); }

void BleMouseService::forceReconnect() {
  const uint16_t handle = connectionHandle_.load();
  if (server_ != nullptr && handle != 0xFFFF) {
    Serial0.println("BLE security timeout; restarting connection");
    server_->disconnect(handle);
  } else {
    NimBLEDevice::startAdvertising();
  }
}

void BleMouseService::move(int8_t dx, int8_t dy, int8_t wheel) {
  portENTER_CRITICAL(&pendingMux_);
  pendingX_ = saturatedAdd(pendingX_, dx);
  pendingY_ = saturatedAdd(pendingY_, dy);
  pendingWheel_ = saturatedAdd(pendingWheel_, wheel);
  reportDirty_ = true;
  portEXIT_CRITICAL(&pendingMux_);
  wakeReportTask();
}

void BleMouseService::pressLeft() { setButton(kLeftButton, true); }
void BleMouseService::releaseLeft() { setButton(kLeftButton, false); }
void BleMouseService::pressRight() { setButton(kRightButton, true); }
void BleMouseService::releaseRight() { setButton(kRightButton, false); }

void BleMouseService::releaseAll() {
  if (buttons_.exchange(0) == 0) {
    return;
  }
  portENTER_CRITICAL(&pendingMux_);
  reportDirty_ = true;
  forceReport_ = true;
  enqueueButtonState(0);
  portEXIT_CRITICAL(&pendingMux_);
  wakeReportTask();
}

void BleMouseService::setButton(uint8_t mask, bool pressed) {
  uint8_t current = buttons_.load();
  uint8_t next = 0;
  do {
    next = pressed ? static_cast<uint8_t>(current | mask)
                   : static_cast<uint8_t>(current & ~mask);
    if (next == current) {
      return;
    }
  } while (!buttons_.compare_exchange_weak(current, next));
  portENTER_CRITICAL(&pendingMux_);
  reportDirty_ = true;
  forceReport_ = true;
  enqueueButtonState(next);
  portEXIT_CRITICAL(&pendingMux_);
  wakeReportTask();
}

void BleMouseService::reportTaskEntry(void* context) {
  static_cast<BleMouseService*>(context)->reportTaskLoop();
}

void BleMouseService::wakeReportTask() {
  if (reportTask_ != nullptr) {
    xTaskNotifyGive(reportTask_);
  }
}

bool BleMouseService::takePending(int8_t& dx, int8_t& dy, int8_t& wheel,
                                  uint8_t& buttons, bool& hasButton,
                                  bool& force) {
  bool dirty = false;
  portENTER_CRITICAL(&pendingMux_);
  dirty = reportDirty_;
  if (dirty) {
    dx = reportDelta(pendingX_);
    dy = reportDelta(pendingY_);
    wheel = reportDelta(pendingWheel_);
    pendingX_ -= dx;
    pendingY_ -= dy;
    pendingWheel_ -= wheel;
    hasButton = buttonQueueSize_ != 0;
    if (hasButton) {
      buttons = buttonQueue_[0];
      for (uint8_t i = 1; i < buttonQueueSize_; ++i) {
        buttonQueue_[i - 1] = buttonQueue_[i];
      }
      --buttonQueueSize_;
    } else {
      buttons = buttons_.load();
    }
    force = forceReport_ || hasButton;
    forceReport_ = false;
    reportDirty_ = pendingX_ != 0 || pendingY_ != 0 || pendingWheel_ != 0 ||
                   buttonQueueSize_ != 0;
  }
  portEXIT_CRITICAL(&pendingMux_);
  return dirty;
}

void BleMouseService::restorePending(int8_t dx, int8_t dy, int8_t wheel,
                                     uint8_t buttons, bool hasButton,
                                     bool force) {
  portENTER_CRITICAL(&pendingMux_);
  pendingX_ += dx;
  pendingY_ += dy;
  pendingWheel_ += wheel;
  if (hasButton) {
    enqueueButtonState(buttons, true);
  }
  reportDirty_ = true;
  forceReport_ = forceReport_ || force;
  portEXIT_CRITICAL(&pendingMux_);
}

void BleMouseService::enqueueButtonState(uint8_t buttons, bool front) {
  if (buttonQueueSize_ >= sizeof(buttonQueue_)) {
    return;
  }
  if (front) {
    for (uint8_t i = buttonQueueSize_; i > 0; --i) {
      buttonQueue_[i] = buttonQueue_[i - 1];
    }
    buttonQueue_[0] = buttons;
  } else {
    buttonQueue_[buttonQueueSize_] = buttons;
  }
  ++buttonQueueSize_;
}

void BleMouseService::reportTaskLoop() {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2));
    const uint32_t nowMs = millis();

    if (!ready_.load() || inputReport_ == nullptr) {
      portENTER_CRITICAL(&pendingMux_);
      pendingX_ = pendingY_ = pendingWheel_ = 0;
      buttonQueueSize_ = 0;
      reportDirty_ = false;
      forceReport_ = false;
      portEXIT_CRITICAL(&pendingMux_);
      consecutiveNotifyFailures_.store(0);
      lastReportMs_ = nowMs;
      continue;
    }

    int8_t dx = 0;
    int8_t dy = 0;
    int8_t wheel = 0;
    uint8_t buttons = 0;
    bool hasButton = false;
    bool force = false;
    if (!takePending(dx, dy, wheel, buttons, hasButton, force)) {
      continue;
    }
    if (!force && static_cast<uint32_t>(nowMs - lastReportMs_) <
                     reportIntervalMs_.load()) {
      restorePending(dx, dy, wheel, buttons, hasButton, force);
      vTaskDelay(pdMS_TO_TICKS(2));
      continue;
    }

    lastReportMs_ = nowMs;
    if (sendReport(buttons, dx, dy, wheel)) {
      consecutiveNotifyFailures_.store(0);
    } else {
      restorePending(dx, dy, wheel, buttons, hasButton, force);
      const uint16_t failures = consecutiveNotifyFailures_.fetch_add(1) + 1;
      if (failures == 1 || failures % 25 == 0) {
        Serial0.printf("BLE notify failed (%u consecutive)\n",
                       failures);
      }
    }
  }
}

bool BleMouseService::sendReport(uint8_t buttons, int8_t dx, int8_t dy,
                                 int8_t wheel) {
  if (!ready_.load() || inputReport_ == nullptr) {
    return false;
  }

  MouseReport report{buttons, dx, dy, wheel};
  // HID input reports use the characteristic value update path. The direct
  // custom-notify overload can be rejected by Windows while its CCCD is being
  // established, even though the HID link is already encrypted.
  inputReport_->setValue(reinterpret_cast<uint8_t*>(&report), sizeof(report));
  return inputReport_->notify();
}

void BleMouseService::updateReportInterval(const NimBLEConnInfo& connection) {
  uint16_t intervalMs = static_cast<uint16_t>(
      (static_cast<uint32_t>(connection.getConnInterval()) * 5U + 3U) / 4U);
  // Keep application traffic below the controller event rate.
  intervalMs = constrain(intervalMs, 20, 30);
  reportIntervalMs_.store(intervalMs);
}

}  // namespace ble
