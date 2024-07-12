/*
 * @Description: 
            TQT-C6 breathing light test program
        The breathing light will gradually become brighter from dark, and gradually
    become darker in a circular manner.
 * @Author: LILYGO_L
 * @Date: 2023-11-14 09:55:07
 * @LastEditTime: 2024-07-11 14:11:48
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void setup()
{
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
    // pinMode(LCD_BL, OUTPUT);
    // ledcAttach(LCD_BL, 2000, 8);
    // ledcWrite(LCD_BL, 0); // 开启屏幕
}
void loop()
{
    delay(2000);
    for (int i = 255; i > 0; i--)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(5);
    }
    for (int i = 0; i < 255; i++)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(2);
    }

    delay(2000);
}