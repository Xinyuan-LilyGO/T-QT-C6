/*
 * @Description: ESP Sleep
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-03-20 16:46:56
 * @License: GPL 3.0
 */
#include "Arduino.h"
#include "pin_config.h"

bool Temp = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Ciallo");

  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_DC, OUTPUT);
  pinMode(LCD_SCLK, OUTPUT);
  pinMode(LCD_MOSI, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_BL, OUTPUT);

  digitalWrite(LCD_CS, Temp);
  digitalWrite(LCD_DC, Temp);
  digitalWrite(LCD_SCLK, Temp);
  digitalWrite(LCD_MOSI, Temp);
  digitalWrite(LCD_RST, Temp);
  digitalWrite(LCD_BL, Temp);
}

void loop() {
  digitalWrite(LCD_CS, Temp);
  digitalWrite(LCD_DC, Temp);
  digitalWrite(LCD_SCLK, Temp);
  digitalWrite(LCD_MOSI, Temp);
  digitalWrite(LCD_RST, Temp);
  digitalWrite(LCD_BL, Temp);

  Temp = !Temp;

  if (Temp == 1) {
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }
  delay(1000);
}
