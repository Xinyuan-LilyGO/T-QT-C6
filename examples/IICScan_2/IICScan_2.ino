/*
 * @Description: 搜索IIC设备
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-16 11:10:02
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-24 08:57:12
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include <Wire.h>
#include "pin_config.h"

void scan_i2c_device(TwoWire &i2c)
{
    Serial.println("Scanning for I2C devices ...");
    Serial.print("      ");
    for (int i = 0; i < 0x10; i++)
    {
        Serial.printf("0x%02X|", i);
    }
    uint8_t error;
    for (int j = 0; j < 0x80; j += 0x10)
    {
        Serial.println();
        Serial.printf("0x%02X |", j);
        for (int i = 0; i < 0x10; i++)
        {
            Wire.beginTransmission(i | j);
            error = Wire.endTransmission();
            if (error == 0)
                Serial.printf("0x%02X|", i | j);
            else
                Serial.print(" -- |");
        }
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    Wire.begin(IIC_SDA, IIC_SCL);
    scan_i2c_device(Wire);
}

void loop()
{
    scan_i2c_device(Wire);
    delay(1000);
}