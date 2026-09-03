/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2023-06-05 13:01:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2026-08-17 16:09:02
 */
#pragma once

#define T_QT_C6_LCD_DRIVER_GC9107 1
#define T_QT_C6_LCD_DRIVER_ST7735 2

// LCD driver selection: uncomment one option only.
#ifndef T_QT_C6_LCD_DRIVER
#define T_QT_C6_LCD_DRIVER T_QT_C6_LCD_DRIVER_GC9107
// #define T_QT_C6_LCD_DRIVER T_QT_C6_LCD_DRIVER_ST7735
#endif

#if T_QT_C6_LCD_DRIVER == T_QT_C6_LCD_DRIVER_GC9107
#define T_QT_C6_LCD_CLASS Arduino_GC9107
#define T_QT_C6_LCD_IPS true
#define T_QT_C6_LCD_COL_OFFSET_1 2
#define T_QT_C6_LCD_ROW_OFFSET_1 1
#define T_QT_C6_LCD_COL_OFFSET_2 0
#define T_QT_C6_LCD_ROW_OFFSET_2 0
#elif T_QT_C6_LCD_DRIVER == T_QT_C6_LCD_DRIVER_ST7735
#define T_QT_C6_LCD_CLASS Arduino_ST7735
#define T_QT_C6_LCD_IPS true
#define T_QT_C6_LCD_COL_OFFSET_1 2
#define T_QT_C6_LCD_ROW_OFFSET_1 3
#define T_QT_C6_LCD_COL_OFFSET_2 2
#define T_QT_C6_LCD_ROW_OFFSET_2 3
#else
#error "Unsupported T-QT-C6 LCD driver selection"
#endif

// #define T_QT_C6_Battery_V1_0_V1_1
#define T_QT_C6_Battery_V1_2

// N085-1212TBWIG06-C08
#define LCD_WIDTH 128
#define LCD_HEIGHT 128
#define LCD_CS 14
#define LCD_DC 19
#define LCD_SCLK 18
#define LCD_MOSI 15
#define LCD_MISO -1
#define LCD_RST 20
#define LCD_BL 2

// Battery
#define BATTERY_MEASUREMENT_CONTROL 8
#define BATTERY_ADC_DATA 6

// Breathing Light
#define BREATHING_LIGHT 9

// IIC
#define IIC_SDA 21
#define IIC_SCL 22

// CST816
#define TP_RST 23
#define TP_INT 7

// SLEEP
#define SLEEP_WAKE_UP_INT 7

// LSM6DSL
#if defined T_QT_C6_Battery_V1_0_V1_1
#define LSM6DSL_IIC_ADDRESS_MODE 3
#elif defined T_QT_C6_Battery_V1_2
#define LSM6DSL_EN 3
#endif

