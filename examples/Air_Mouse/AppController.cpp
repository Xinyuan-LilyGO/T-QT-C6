#include "AppController.h"

#include "BoardConfig.h"

namespace app {

AppController::AppController()
    : touch_(Wire),
      imu_(Wire),
      power_(Wire),
      motion_(imu_, cursor_, mouse_),
      lvgl_(display_) {}

//process:i2c->pmu->imu->lvgl->imu_cali->touch
bool AppController::begin() {
  Serial0.begin(board::kDebugBaud, SERIAL_8N1, board::kDebugUartRx,
               board::kDebugUartTx);
  delay(100);
  Serial0.println("T-QT-C6 Air Mouse starting");
  Serial0.printf("Debug UART: RX=%d TX=%d baud=%lu\n", board::kDebugUartRx,
                 board::kDebugUartTx, static_cast<unsigned long>(board::kDebugBaud));
  Serial0.printf("I2C pins: SDA=%d SCL=%d freq=%lu\n", board::kI2cSda,
                board::kI2cScl, static_cast<unsigned long>(board::kI2cFrequency));

  const bool i2cOk = Wire.begin(board::kI2cSda, board::kI2cScl,
                                board::kI2cFrequency);
  Serial0.printf("I2C begin: %s\n", i2cOk ? "OK" : "FAILED");
  if (!i2cOk) {
    Serial0.println("Bottom module check failed: I2C unavailable");
    return false;
  }

  const bool pmuOk = power_.begin();
  Serial0.printf("PMU: %s, charging=%s, battery=%u%% raw=%u\n",
                 pmuOk ? "OK" : "FAILED", power_.charging() ? "YES" : "NO",
                 power_.batteryPercent(),
                 power_.batteryRaw());
  if (!pmuOk) {
    Serial0.println("Bottom module check failed: SGM41562 not detected");
    return false;
  }

  imuOk_ = imu_.begin();
  Serial0.printf("IMU: %s\n", imuOk_ ? "OK" : "FAILED");
  if (!imuOk_) {
    Serial0.println("Bottom module check failed: LSM6DSL not detected");
    return false;
  }

  if (!display_.begin()) {
    Serial0.println("Display initialization failed");
    return false;
  }
  Serial0.printf("Display initialized: rotation=%u size=%ux%u\n",
                board::kDisplayRotation, board::kScreenWidth,
                board::kScreenHeight);
  if (!lvgl_.begin()) {
    Serial0.println("LVGL initialization failed");
    return false;
  }
  ui_.begin();
  ui_.setBattery(power_.batteryPercent(), power_.charging());
  ui_.showCalibrating();
  lvgl_.task(millis());

  if (imuOk_) {
    Serial0.println("Keep the device still: calibrating IMU");
    bool calibrated = false;
    for (uint8_t attempt = 1; attempt <= 3 && !calibrated; ++attempt) {
      calibrated = imu_.calibrate();
      if (!calibrated) {
        Serial0.printf("IMU calibration rejected (attempt %u/3)\n", attempt);
        delay(250);
      }
    }
    imuOk_ = calibrated;
    if (imuOk_) {
      const bool streaming = imu_.startStreaming();
      Serial0.printf("IMU FIFO: %s\n", streaming ? "ON" : "OFF (fallback)");
    }
  }

  // The V1.2 battery backboard powers the sensor section from GPIO3. Probe
  // touch only after that rail has been enabled and the I2C bus is live.
  touchOk_ = touch_.begin();
  Serial0.printf("Touch: %s\n", touchOk_ ? "OK" : "FAILED");

  ui_.showPairing(false);
  lvgl_.task(millis());

  if (!mouse_.begin("TQT-C6 Air Mouse")) {
    Serial0.println("BLE HID initialization failed");
    return false;
  }

  if (!motion_.begin()) {
    Serial0.println("IMU motion task creation failed");
    return false;
  }

  if (!power_.startMonitoring()) {
    Serial0.println("Power monitor task creation failed");
    return false;
  }

  Serial0.println("BLE advertising started");
  return true;
}

void AppController::loop() {
  const uint32_t nowMs = millis();
  const bool ready = mouse_.ready();
  lvgl_.task(nowMs, ready ? 25 : 5);
  updateBatteryUi(nowMs);

  const bool linkConnected = mouse_.linkConnected();
  handleConnectionState(ready, linkConnected);

  if (linkConnected && !ready) {
    if (notReadySinceMs_ == 0) {
      notReadySinceMs_ = nowMs;
    } else if (!reconnectRequested_ && nowMs - notReadySinceMs_ >= 8000U) {
      reconnectRequested_ = true;
      mouse_.forceReconnect();
    }
  } else {
    notReadySinceMs_ = 0;
    reconnectRequested_ = false;
  }

  if (ready && touchOk_) {
    touch_.update(nowMs);
    input::ButtonEvent event{};
    if (touchButtons_.update(touch_.point(), nowMs, event)) {
      handleButtonEvent(event, nowMs);
    }
  }

  motion_.setActive(ready && imuOk_);
  delay(1);
}

void AppController::updateBatteryUi(uint32_t nowMs) {
  if (nowMs - lastBatteryUiMs_ < 1000U) {
    return;
  }
  lastBatteryUiMs_ = nowMs;
  ui_.setBattery(power_.batteryPercent(), power_.charging());
}

void AppController::handleConnectionState(bool ready, bool linkConnected) {
  if (ready != lastReady_) {
    mouse_.releaseAll();
    touchButtons_.reset();
    ui_.setButtonPressed(input::MouseButton::Left, false);
    ui_.setButtonPressed(input::MouseButton::Right, false);

    if (ready) {
      ui_.showControl(imuOk_, touchOk_);
    } else {
      ui_.showPairing(linkConnected);
    }
    lastReady_ = ready;
  } else if (!ready && linkConnected != lastLinkConnected_) {
    ui_.showPairing(linkConnected);
  }
  lastLinkConnected_ = linkConnected;
}

//stimulate mouse clicking
void AppController::handleButtonEvent(const input::ButtonEvent& event,
                                      uint32_t nowMs) {
  const bool pressed = event.type == input::ButtonEventType::Pressed;
  ui_.setButtonPressed(event.button, pressed);

  if (event.button == input::MouseButton::Left) {
    if (pressed) {
      mouse_.pressLeft();
    } else {
      mouse_.releaseLeft();
    }
  } else if (event.button == input::MouseButton::Right) {
    if (pressed) {
      mouse_.pressRight();
    } else {
      mouse_.releaseRight();
    }
  }

  // Suppress the brief gyro impulse caused by touching the glass.
  motion_.freezeUntil(nowMs + 20);
}

}  // namespace app
