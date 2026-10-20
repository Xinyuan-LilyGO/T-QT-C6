#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace drivers {

class I2cRegisterDevice {
 public:
  I2cRegisterDevice(TwoWire& wire, uint8_t address);

 protected:
  bool probe() const;
  bool writeRegister(uint8_t reg, uint8_t value) const;
  bool readRegister(uint8_t reg, uint8_t& value) const;
  bool readRegisters(uint8_t startReg, uint8_t* data, size_t length) const;
  void setAddress(uint8_t address) { address_ = address; }

  TwoWire& wire_;
  uint8_t address_;
};

}  // namespace drivers
