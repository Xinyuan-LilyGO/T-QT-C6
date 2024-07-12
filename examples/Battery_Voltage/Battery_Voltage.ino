/*
 * @Description: 
            Example of measuring T-QT-C6 battery voltage
        Enable voltage measurement when measuring voltage, and serial port will output
    voltage information.
 * @Author: LILYGO_L
 * @Date: 2023-07-14 16:16:07
 * @LastEditTime: 2024-07-11 14:11:43
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

static bool Temp = false;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void setup()
{
    Serial.begin(115200);

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
    digitalWrite(BATTERY_MEASUREMENT_CONTROL, LOW); // 开启电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    // pinMode(LCD_BL, OUTPUT);
    // ledcAttach(LCD_BL, 2000, 8);
    // ledcWrite(LCD_BL, 0); // 开启屏幕
}

void loop()
{
    if (Temp == false)
    {
        Serial.print("Turn on battery voltage measurement\n");
    }
    else
    {
        Serial.print("Turn off battery voltage measurement\n");
    }
    Serial.print("ADC Value:");
    Serial.println(analogRead(BATTERY_ADC_DATA));

    Serial.printf("ADC Voltage: %.03f V\n", ((float)analogReadMilliVolts(BATTERY_ADC_DATA)) / 1000.0);

    Serial.printf("Battery Voltage: %.03f V\n", (((float)analogReadMilliVolts(BATTERY_ADC_DATA)) / 1000.0) * 2.0);
    Serial.println();

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(5);
    }
    for (int i = 0; i <= 255; i++)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(2);
    }

    if (Temp == false)
    {
        digitalWrite(BATTERY_MEASUREMENT_CONTROL, HIGH); // 关闭电池电压测量
    }
    else
    {
        digitalWrite(BATTERY_MEASUREMENT_CONTROL, LOW); // 开启电池电压测量
    }

    Temp = !Temp;
    delay(1000);
}