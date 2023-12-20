/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2023-06-05 13:01:59
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-18 10:10:12
 */
#pragma once

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
#define Battery_Measurement_Control 8
#define Battery_ADC_Data 6

// Breathing Light
#define Breathing_Light 9

// IIC
#define IIC_SDA 21
#define IIC_SCL 22

// CST816
#define CST816_Address 0x15
#define TP_RST 23
#define TP_INT 7

// SLEEP
#define SLEEP_WAKE_UP_INT 7
