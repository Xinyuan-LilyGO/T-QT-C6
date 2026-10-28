#include "I2cRegisterDevice.h"

#include "I2cBusLock.h"

namespace drivers {

I2cRegisterDevice::I2cRegisterDevice(TwoWire& wire, uint8_t address)
    : wire_(wire), address_(address) {}

bool I2cRegisterDevice::probe() const {
  I2cBusLock lock;
  if (!lock.locked()) {
    return false;
  }
  wire_.beginTransmission(address_);
  return wire_.endTransmission() == 0;
}

bool I2cRegisterDevice::writeRegister(uint8_t reg, uint8_t value) const {
  I2cBusLock lock;
  if (!lock.locked()) {
    return false;
  }
  wire_.beginTransmission(address_);
  wire_.write(reg);
  wire_.write(value);
  return wire_.endTransmission() == 0;
}

bool I2cRegisterDevice::readRegister(uint8_t reg, uint8_t& value) const {
  return readRegisters(reg, &value, 1);
}

bool I2cRegisterDevice::readRegisters(uint8_t startReg, uint8_t* data,
                                      size_t length) const {
  I2cBusLock lock;
  if (!lock.locked()) {
    return false;
  }
  wire_.beginTransmission(address_);
  wire_.write(startReg);
  if (wire_.endTransmission(false) != 0) {
    return false;
  }

  const size_t received = wire_.requestFrom(address_, length, true);
  if (received != length) {
    while (wire_.available()) {
      wire_.read();
    }
    return false;
  }

  for (size_t i = 0; i < length; ++i) {
    data[i] = static_cast<uint8_t>(wire_.read());
  }
  return true;
}

}  // namespace drivers
