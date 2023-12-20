/*
 * @Description(CN): TQT-C6呼吸灯测试程序
 *      呼吸灯会由暗逐渐变亮，由亮逐渐变暗循环
 * 
 * @Description(EN): TQT-C6 breathing light test program
 *      The breathing light will gradually become brighter from dark, and gradually 
 *  become darker in a circular manner.
 * 
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-14 09:55:07
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 11:06:16
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "pin_config.h"

void setup()
{
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
    // pinMode(LCD_BL, OUTPUT);
    // ledcAttach(LCD_BL, 20000, 8);
    // ledcWrite(LCD_BL, 0); // 开启屏幕
}
void loop()
{
    delay(2000);
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

    delay(2000);
}