#include "TouchDevice.h"

#include "BoardConfig.h"

namespace drivers {

TouchDevice::TouchDevice(TwoWire& wire)
    : I2cRegisterDevice(wire, board::kTouchAddress) {}

bool TouchDevice::begin() {
  Serial0.printf("Touch init: reset=%d irq=%d candidates=0x%02X/0x%02X\n",
                board::kTouchReset, board::kTouchInterrupt,
                board::kTouchAddress, board::kTouchAlternateAddress);
  pinMode(board::kTouchInterrupt, INPUT_PULLUP);
  pinMode(board::kTouchReset, OUTPUT);
  digitalWrite(board::kTouchReset, HIGH);
  delay(5);
  digitalWrite(board::kTouchReset, LOW);
  delay(20);
  digitalWrite(board::kTouchReset, HIGH);
  delay(200);

  Serial0.print("I2C scan:");
  uint8_t deviceCount = 0;
  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    wire_.beginTransmission(address);
    if (wire_.endTransmission() == 0) {
      Serial0.printf(" 0x%02X", address);
      ++deviceCount;
    }
  }
  Serial0.printf(" (%u device%s)\n", deviceCount,
                deviceCount == 1 ? "" : "s");

  uint8_t chipId = 0;
  healthy_ = false;
  const uint8_t addresses[] = {board::kTouchAddress,
                               board::kTouchAlternateAddress};
  for (const uint8_t address : addresses) {
    setAddress(address);
    const bool addressed = probe();
    Serial0.printf("Touch probe 0x%02X: %s\n", address,
                  addressed ? "ACK" : "NACK");
    const bool chipRead = addressed && readRegister(kChipIdReg, chipId);
    Serial0.printf("Touch chip-id 0x%02X: %s", address,
                  chipRead ? "OK" : "FAILED");
    if (chipRead) {
      Serial0.printf(" value=0x%02X", chipId);
    }
    Serial0.println();
    if (chipRead) {
      healthy_ = true;
      Serial0.printf("Touch controller: addr=0x%02X chip=0x%02X\n", address,
                    chipId);
      break;
    }
  }
  if (!healthy_) {
    Serial0.println("Touch controller not found at 0x15 or 0x38");
  }
  return healthy_;
}

void TouchDevice::update(uint32_t nowMs) {
  if (nowMs - lastPollMs_ < kPollIntervalMs) {
    return;
  }
  lastPollMs_ = nowMs;

  uint8_t data[5]{};
  if (!readRegisters(kFingerCountReg, data, sizeof(data))) {
    if (point_.down && nowMs - lastValidTouchMs_ >= kReadFailureReleaseMs) {
      point_.down = false;
    }
    return;
  }

  healthy_ = true;
  const uint8_t fingers = data[0] & 0x0F;
  if (fingers == 0) {
    point_.down = false;
    return;
  }

  const uint16_t x = static_cast<uint16_t>(((data[1] & 0x0F) << 8) | data[2]);
  const uint16_t y = static_cast<uint16_t>(((data[3] & 0x0F) << 8) | data[4]);
  const uint16_t clampedX = constrain(x, 0, board::kScreenWidth - 1);
  const uint16_t clampedY = constrain(y, 0, board::kScreenHeight - 1);
  // The controller X axis already matches the physical left/right layout.
  // Mirroring it makes a physical left press become the right button.
  point_.x = clampedX;
  point_.y = board::kScreenHeight - 1 - clampedY;
  point_.down = true;
  lastValidTouchMs_ = nowMs;
}

const TouchPoint& TouchDevice::point() const { return point_; }

}  // namespace drivers
