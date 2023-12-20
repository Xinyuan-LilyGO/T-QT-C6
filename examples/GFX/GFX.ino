/*
 * @Description: GFX.ino
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-07-25 13:45:02
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 13:51:20
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

// N085-1212TBWIG06-C08
Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    2 /* col offset 1 */, 1 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    // 呼吸灯
    pinMode(Breathing_Light, OUTPUT);
    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 255); // 关闭呼吸灯

    // 测量电池
    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 20000, 8);
    ledcWrite(LCD_BL, 0); // 开启屏幕

    gfx->begin();
    gfx->fillScreen(PINK);

    gfx->setCursor(60, 60);
    gfx->setTextColor(YELLOW);
    gfx->println("Ciallo");
}

void loop()
{
    for (int i = 255; i > 0; i--)
    {
        ledcWrite(LCD_BL, i);
        delay(5);
    }
    delay(1000);
    for (int i = 0; i <= 255; i++)
    {
        ledcWrite(LCD_BL, i);
        delay(2);
    }

    delay(2000);
}