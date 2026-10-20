#pragma once

#include <Arduino.h>

// 1 = early GC9107 panel, 2 = newer ST7735 panel.
#ifndef TQT_C6_LCD_DRIVER
#define TQT_C6_LCD_DRIVER 1
#endif

// 1 = Battery V1.2 (GPIO3 enables the IMU), 0 = Battery V1.0/V1.1
// (GPIO3 selects the LSM6DSL 0x6A address).
#ifndef TQT_C6_BATTERY_V12
#define TQT_C6_BATTERY_V12 1
#endif

namespace board {

constexpr uint16_t kScreenWidth = 128;
constexpr uint16_t kScreenHeight = 128;
constexpr uint8_t kDisplayRotation = 1;

constexpr int8_t kLcdCs = 14;
constexpr int8_t kLcdDc = 19;
constexpr int8_t kLcdSclk = 18;
constexpr int8_t kLcdMosi = 15;
constexpr int8_t kLcdMiso = -1;
constexpr int8_t kLcdReset = 20;
constexpr int8_t kLcdBacklight = 2;

constexpr int8_t kI2cSda = 21;
constexpr int8_t kI2cScl = 22;
constexpr uint32_t kI2cFrequency = 400000;

// ESP32-C6 UART0 pins used by the ROM boot log and external CH340 adapter.
constexpr int8_t kDebugUartRx = 17;
constexpr int8_t kDebugUartTx = 16;
constexpr uint32_t kDebugBaud = 115200;

constexpr int8_t kPmuInterrupt = 4;
constexpr int8_t kBatteryMeasurementControl = 8;
constexpr int8_t kBatteryAdc = 6;

constexpr int8_t kTouchReset = 23;
constexpr int8_t kTouchInterrupt = 7;
constexpr uint8_t kTouchAddress = 0x15;
constexpr uint8_t kTouchAlternateAddress = 0x38;

constexpr int8_t kImuControl = 3;
constexpr uint8_t kImuAddress = 0x6A;

constexpr uint8_t kBacklightOffDuty = 255;
constexpr uint8_t kBacklightOnDuty = 0;

}  // namespace board
