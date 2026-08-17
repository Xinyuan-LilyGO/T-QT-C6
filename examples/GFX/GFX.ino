/*
 * @Description: GFX.ino
 * @Author: LILYGO_L
 * @Date: 2023-07-25 13:45:02
 * @LastEditTime: 2026-08-17 15:40:10
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new T_QT_C6_LCD_CLASS(
    bus, LCD_RST /* RST */, 0 /* rotation */, T_QT_C6_LCD_IPS /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    T_QT_C6_LCD_COL_OFFSET_1 /* col offset 1 */, T_QT_C6_LCD_ROW_OFFSET_1 /* row offset 1 */,
    T_QT_C6_LCD_COL_OFFSET_2 /* col offset 2 */, T_QT_C6_LCD_ROW_OFFSET_2 /* row offset 2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    if (ETA4662->begin() == true)
    {
        Serial.println("ETA4662 initialization successfully");
    }
    else if (SGM41562->begin() == true)
    {
        Serial.println("SGM41562 initialization successfully");
    }
    else
    {
        Serial.println("Power chip initialization failed");
    }

    // 呼吸灯
    pinMode(BREATHING_LIGHT, OUTPUT);
    ledcAttach(BREATHING_LIGHT, 2000, 8);
    ledcWrite(BREATHING_LIGHT, 255); // 关闭呼吸灯

    // 测量电池
    pinMode(BATTERY_ADC_DATA, INPUT_PULLDOWN);
    pinMode(BATTERY_MEASUREMENT_CONTROL, OUTPUT);
    digitalWrite(BATTERY_MEASUREMENT_CONTROL, HIGH); // 关闭电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 2000, 8);
    ledcWrite(LCD_BL, 0); // 开启屏幕

    gfx->begin();
}

void loop()
{
    static const uint16_t colors[] = {RED, GREEN, BLUE, WHITE, BLACK};

    for (const uint16_t color : colors)
    {
        gfx->fillScreen(color);
        delay(1000);
    }
}
