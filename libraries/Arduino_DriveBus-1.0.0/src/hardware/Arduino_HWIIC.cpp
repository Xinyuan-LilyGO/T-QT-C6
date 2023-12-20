/*
 * @Description: Arduino_HWIIC.cpp
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-11-16 15:46:16
 * @LastEditors: Xk_w
 * @LastEditTime: 2023-11-22 14:16:22
 * @License: GPL 3.0
 */
#include "Arduino_HWIIC.h"

Arduino_HWIIC::Arduino_HWIIC(int8_t sda, int8_t scl, TwoWire *wire)
    : _sda(sda), _scl(scl), _wire(wire)
{
}

bool Arduino_HWIIC::begin(int32_t speed)
{
    _speed = (speed == DRIVEBUS_DEFAULT_VALUE) ? IIC_Default_Speed : speed;

    if (_wire->begin(_sda, _scl, _speed) == false)
    {
        return false;
    }
    return true;
}

void Arduino_HWIIC::BeginTransmission(uint8_t device_address)
{
    _wire->beginTransmission(device_address);
}

bool Arduino_HWIIC::EndTransmission(void)
{
    return !(_wire->endTransmission());
}

bool Arduino_HWIIC::Write(uint8_t d)
{
    return _wire->write(d);
}

uint8_t Arduino_HWIIC::Read(void)
{
    return (uint8_t)_wire->read();
}

bool Arduino_HWIIC::RequestFrom(uint8_t device_address, size_t length)
{
    return _wire->requestFrom(device_address, length);
}