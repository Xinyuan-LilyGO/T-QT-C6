/*
 * @Description: ESP Sleep
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditTime: 2024-11-22 16:37:31
 * @License: GPL 3.0
 */
#include "Arduino.h"
#include "pin_config.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "cpp_bus_driver_library.h"

#define SLEEP_WAKE_UP_INT GPIO_NUM_7

static size_t CycleTime = 0;

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new T_QT_C6_LCD_CLASS(
    bus, LCD_RST /* RST */, 0 /* rotation */, T_QT_C6_LCD_IPS /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    T_QT_C6_LCD_COL_OFFSET_1 /* col offset 1 */, T_QT_C6_LCD_ROW_OFFSET_1 /* row offset 1 */,
    T_QT_C6_LCD_COL_OFFSET_2 /* col offset 2 */, T_QT_C6_LCD_ROW_OFFSET_2 /* row offset 2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> LSM6DSL(new Arduino_LSM6DSL(IIC_Bus, LSM6DSL_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

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
    Serial.begin(115200);
    Serial.println("Ciallo");

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

    pinMode(LSM6DSL_IIC_ADDRESS_MODE, OUTPUT);
    digitalWrite(LSM6DSL_IIC_ADDRESS_MODE, LOW); // 设置LSM6DSL的IIC地址为0x6A

    pinMode(SLEEP_WAKE_UP_INT, INPUT_PULLUP);

    // 屏幕背光
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 2000, 8);
    ledcWrite(LCD_BL, 0); // 开启屏幕

    if (LSM6DSL->begin() == false)
    {
        Serial.println("LSM6DSL initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("LSM6DSL initialization successfully");
    }

    gfx->begin();
    gfx->fillScreen(RGB565_WHITE);

    Serial.println("Touch the screen to enter light sleep");
    gfx->setCursor(10, 64);
    gfx->setTextColor(RGB565_BLACK);
    gfx->println("Touch the screen to enter light sleep");
}

void loop()
{
    if (digitalRead(SLEEP_WAKE_UP_INT) == LOW)
    {
        delay(300);

        gfx->fillScreen(RGB565_WHITE);

        Serial.println("Enter light sleep");

        gfx->setCursor(10, 64);
        gfx->println("Enter light sleep");

        // 设置陀螺仪传感器为休眠模式
        LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_SLEEP_MODE,
                                        LSM6DSL->Arduino_IIC_IMU::Device_State::IMU_DEVICE_ON);
        // 设置加速度传感器性能模式为下电模式
        LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_ACCELERATION_POWER_MODE,
                                        LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_OFF_POWER);
        // 设置陀螺仪传感器性能模式为下电模式
        LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_POWER_MODE,
                                        LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_OFF_POWER);

        ledcWrite(LCD_BL, 255); // 关闭屏幕
        gfx->displayOff();

        delay(1000);

        esp_light_sleep_start();
    }
}
