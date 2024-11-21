/*
 * @Description: ESP Sleep
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditTime: 2024-07-11 14:04:08
 * @License: GPL 3.0
 */
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    128 /* width */, 128 /* height */,
    2 /* col offset 1 */, 1 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(IIC_Bus, CST816T_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

std::unique_ptr<Arduino_IIC> LSM6DSL(new Arduino_LSM6DSL(IIC_Bus, LSM6DSL_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816T->IIC_Interrupt_Flag = true;
}

void setup()
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

    pinMode(LSM6DSL_IIC_ADDRESS_MODE, OUTPUT);
    digitalWrite(LSM6DSL_IIC_ADDRESS_MODE, LOW); // 设置LSM6DSL的IIC地址为0x6A

    pinMode(BREATHING_LIGHT, OUTPUT);
    digitalWrite(BREATHING_LIGHT, HIGH);

    pinMode(BATTERY_ADC_DATA, INPUT_PULLDOWN);
    pinMode(BATTERY_MEASUREMENT_CONTROL, OUTPUT);
    digitalWrite(BATTERY_MEASUREMENT_CONTROL, HIGH);

    ledcAttach(BREATHING_LIGHT, 2000, 8);
    ledcWrite(BREATHING_LIGHT, 255); // brightness 0 - 255

    ledcAttach(LCD_BL, 2000, 8);
    ledcWrite(LCD_BL, 0); // brightness 0 - 255

    if (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("CST816T initialization successfully");
    }

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
    gfx->fillScreen(WHITE);

    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->println("5 seconds into light sleep");

    delay(5000);

    // 设置陀螺仪传感器为休眠模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_SLEEP_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_State::IMU_DEVICE_ON);
    // 设置加速度传感器性能模式为下电模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_ACCELERATION_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_OFF_POWER);
    // 设置陀螺仪传感器性能模式为下电模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_OFF_POWER);

    ledcWrite(LCD_BL, 255);
    gfx->displayOff();

    // esp_deep_sleep_enable_gpio_wakeup(SLEEP_WAKE_UP_INT, ESP_GPIO_WAKEUP_GPIO_LOW);
    // esp_sleep_enable_ext1_wakeup(SLEEP_WAKE_UP_INT, ESP_EXT1_WAKEUP_ANY_LOW);
    esp_sleep_enable_timer_wakeup(20000000);
    esp_light_sleep_start();
}

void loop()
{
    if (CST816T->IIC_Interrupt_Flag == true)
    {
        CST816T->IIC_Interrupt_Flag = false;

        gfx->begin();
        gfx->displayOn();
        gfx->fillScreen(WHITE);

        gfx->setCursor(10, 60);
        gfx->setTextSize(1);
        gfx->setTextColor(BLACK);
        gfx->println("Light sleep wake-up successful");

        Serial.println("Light sleep wake-up successful");

        ledcWrite(LCD_BL, 0); // brightness 0 - 255
    }
}
