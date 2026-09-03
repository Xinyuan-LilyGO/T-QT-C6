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
#include "cpp_bus_driver_library.h"
#include "pin_config.h"

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::shared_ptr<cpp_bus_driver::HardwareI2c1> CPP_IIC_Master_Bus =
    std::make_shared<cpp_bus_driver::HardwareI2c1>(IIC_SDA, IIC_SCL);
std::shared_ptr<cpp_bus_driver::HardwareI2c1> SGM41562_IIC_Bus =
    std::make_shared<cpp_bus_driver::HardwareI2c1>(CPP_IIC_Master_Bus);
std::unique_ptr<cpp_bus_driver::Sgm41562xx> SGM41562(
    new cpp_bus_driver::Sgm41562xx(SGM41562_IIC_Bus));

void setup()
{
    i2c_master_bus_handle_t sgm41562_bus_handle = nullptr;
    if (ETA4662->begin() == true)
    {
        Serial.println("ETA4662 initialization successfully");
    }
    else if (i2c_master_get_bus_handle(
                 I2C_NUM_0, &sgm41562_bus_handle) == ESP_OK &&
             CPP_IIC_Master_Bus->set_bus_handle(sgm41562_bus_handle) &&
             SGM41562->Init())
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
