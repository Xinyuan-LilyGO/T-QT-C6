/*
 * @Description: 
 *      该程序将使TQT-C6进入light_sleep模式后定时10秒退出
 *
 * @Description: 
 *      The program will put TQT-C6 into light_sleep mode and exit after 10 seconds.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 14:13:03
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

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816T->IIC_Interrupt_Flag = true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(Breathing_Light, OUTPUT);
    digitalWrite(Breathing_Light, HIGH);

    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, HIGH);

    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 255); // brightness 0 - 255

    ledcAttach(LCD_BL, 20000, 8);
    ledcWrite(LCD_BL, 0); // brightness 0 - 255

    while (ETA4662->begin() == false)
    {
        Serial.println("ETA4662 initialization fail");
        delay(2000);
    }

    while (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    Serial.println("CST816T initialization successfully");

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(MAGENTA);
    gfx->println("5 seconds into sleep and 10 seconds to exit");

    delay(5000);

    gfx->displayOff();
    ledcWrite(LCD_BL, 255);

    // esp_sleep_enable_ext1_wakeup(SLEEP_WAKE_UP_INT, ESP_EXT1_WAKEUP_ANY_LOW);
    esp_sleep_enable_timer_wakeup(10000000);
    esp_light_sleep_start();
    // esp_deep_sleep_start();
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
        gfx->setTextColor(MAGENTA);
        gfx->println("Light sleep wake-up successful");

        Serial.println("Light sleep wake-up successful");

        ledcWrite(LCD_BL, 0); // brightness 0 - 255
    }
}
