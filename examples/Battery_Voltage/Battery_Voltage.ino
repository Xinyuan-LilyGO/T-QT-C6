/*
 * @Description(CN): 测量TQT-C6电池电压示例
 *      在测量电压的时候需要开启电压测量，串口会输出电电压信息
 *
 * @Description(EN): Example of measuring TQT-C6 battery voltage
 *      Enable voltage measurement when measuring voltage, and serial port will output
 *  voltage information.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-07-14 16:16:07
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 14:16:44
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "pin_config.h"

static bool Temp = false;

void setup()
{
    Serial.begin(115200);

    // 呼吸灯
    pinMode(Breathing_Light, OUTPUT);
    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 255); // 关闭呼吸灯

    // 测量电池
    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, LOW); // 开启电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    // pinMode(LCD_BL, OUTPUT);
    // ledcAttach(LCD_BL, 20000, 8);
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
    Serial.println(analogRead(Battery_ADC_Data));

    Serial.printf("ADC Voltage: %.03f V\n", ((float)analogReadMilliVolts(Battery_ADC_Data)) / 1000.0);

    Serial.printf("Battery Voltage: %.03f V\n", (((float)analogReadMilliVolts(Battery_ADC_Data)) / 1000.0) * 2.0);
    Serial.println();

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(Breathing_Light, i);
        delay(5);
    }
    for (int i = 0; i < 255; i++)
    {
        ledcWrite(Breathing_Light, i);
        delay(2);
    }

    if (Temp == false)
    {
        digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量
    }
    else
    {
        digitalWrite(Battery_Measurement_Control, LOW); // 开启电池电压测量
    }

    Temp = !Temp;
    delay(1000);
}