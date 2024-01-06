/*
 * @Description(CN):
 *      该程序用于测试TQT-C6正常工作时的最高功耗
 *
 * @Description(EN):
 *      This program is used to test the maximum power consumption of
 *  TQT-C6 during normal operation.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-01-06 15:12:02
 * @License: GPL 3.0
 */
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX 5000

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

bool Wifi_Connection_State;

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

void WIFI_STA_Test_Loop(void)
{
    gfx->fillScreen(BLACK);
    gfx->setCursor(0, 0);
    gfx->setTextSize(1);
    gfx->setTextColor(GREEN);

    String temp;
    int wifi_num = 0;
    uint64_t last_tick;

    temp = "Scanning wifi";
    gfx->print(temp);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        temp = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        temp = "\nWiFi scan complete !\n";
        temp += wifi_num;
        temp += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            temp += (i + 1);
            temp += ": ";
            temp += WiFi.SSID(i);
            temp += " (";
            temp += WiFi.RSSI(i);
            temp += ")";
            temp += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    gfx->print(temp);

    temp = "Connecting to ";
    temp += WIFI_SSID;
    temp += "\n";

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    last_tick = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        temp += ".";

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_State = false;
            break;
        }
        else
        {
            Wifi_Connection_State = true;
        }
        delay(500); // 这个延时必须要有
    }

    if (Wifi_Connection_State == true)
    {
        temp += "\nThe connection was successful ! \nTakes: ";
        temp += (String)(millis() - last_tick);
        temp += " ms";
    }
    else
    {
        temp += "\nThe connection was fail !";
    }

    gfx->fillScreen(BLACK);
    gfx->setCursor(0, 0);
    gfx->print(temp);
}

void WIFI_Time(void)
{
    String temp;
    gfx->fillRect(0, 0, 128, 60, WHITE);
    gfx->setCursor(0, 10);

    if (Wifi_Connection_State == true)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 10000))
        {
            temp = "Failed to obtain time!";
        }
        else
        {
            temp = "Get time success";
            temp += "\nYear/Month: ";
            temp += (String)(timeinfo.tm_year + 1900);
            temp += "/";
            temp += (String)(timeinfo.tm_mon + 1);
            temp += "/";
            temp += (String)timeinfo.tm_mday;
            temp += "\nTime: ";
            temp += (String)timeinfo.tm_hour;
            temp += " : ";
            temp += (String)timeinfo.tm_min;
            temp += " : ";
            temp += (String)timeinfo.tm_sec;
        }
    }
    else
    {
        temp += "\n\nNot connected to the network";
    }

    gfx->print(temp);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(Breathing_Light, OUTPUT);
    digitalWrite(Breathing_Light, LOW); // 开启呼吸灯
    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 0); // brightness 0 - 255
    analogReadResolution(12);

    ledcAttach(LCD_BL, 20000, 8);
    ledcWrite(LCD_BL, 0); // brightness 0 - 255

    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, LOW); // 开启电池电压测量

    while (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    Serial.println("CST816T initialization successfully");

    while (ETA4662->begin() == false)
    {
        Serial.println("ETA4662 initialization fail");
        delay(2000);
    }
    Serial.println("ETA4662 initialization successfully");

    // 注意！ 这里设置充电功能开，必须要接电池，否则系统电源总线将断开一次
    // 注意！ 这里设置充电功能开，必须要接电池，否则系统电源总线将断开一次
    // 注意！ 这里设置充电功能开，必须要接电池，否则系统电源总线将断开一次
    ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                    ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON); // 充电
    // ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_WATCHDOG_MODE,
    //                                 ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);                 // 看门狗（当启动ETA4662的看门狗时，看门狗的定时器到达指定值后将断开电源重新连接，与ETA4662通信的MCU将重启）
    // ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_WATCHDOG_TIMER, 80); // 看门狗定时器值

    // 热调节阈值设置为60度
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 60);
    // 最小输入电压设置为4760mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_INPUT_VOLTAGE_LIMIT, 4760);
    // 充电目标电压电压设置为4215mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4215);
    // 系统电压设置为4200mV（输出电压，该值不能设置过高）
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_SYSTEM_VOLTAGE_LIMIT, 4200);
    // 输入电流限制设置为500mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 500);
    // 快速充电电流限制设置为456mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 456);
    // 终端充电和预充电电流限制设置为5mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT, 5);
    // BAT到SYS的放电电流限制设置为2200mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT, 2200);

    gfx->begin();
    gfx->fillScreen(BLACK);

    WIFI_STA_Test_Loop();

    delay(2000);

    if (Wifi_Connection_State == true)
    {
        // Obtain and set the time from the network time server
        // After successful acquisition, the chip will use the RTC clock to update the holding time
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

        delay(3000);
    }

    gfx->fillRect(0, 60, 128, 64, WHITE);
    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(MAGENTA);
}

void loop()
{
    if (Wifi_Connection_State == true)
    {
        WIFI_Time();
    }
    else
    {
        gfx->fillRect(0, 0, 128, 60, WHITE);
        gfx->setCursor(0, 10);
        gfx->print("Wifi Connection Fail");
    }

    gfx->setCursor(0, 40);
    gfx->printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    Serial.print("ADC Value:");
    Serial.println(analogRead(Battery_ADC_Data));

    Serial.printf("ADC Voltage: %.03f V\n", ((float)analogReadMilliVolts(Battery_ADC_Data)) / 1000.0);

    Serial.printf("Battery Voltage: %.03f V\n", (((float)analogReadMilliVolts(Battery_ADC_Data)) / 1000.0) * 2.0);
    Serial.println();

    if (CST816T->IIC_Interrupt_Flag == true)
    {
        CST816T->IIC_Interrupt_Flag = false;

        gfx->fillRect(0, 68, 128, 64, WHITE);
        gfx->setCursor(90, 90);
        gfx->setTextColor(MAGENTA);

        if (CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID) == "Swipe Up")
        {
            gfx->printf("\nGesture:Swipe Down\n");
        }
        else if (CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID) == "Swipe Down")
        {
            gfx->printf("\nGesture:Swipe Up\n");
        }
        else
        {
            gfx->printf("\nGesture:%s\n",
                        (CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID)).c_str());
        }

        gfx->printf("Fingers Number:%d\n",
                    CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER));
        gfx->printf("Touch X:%d Y:%d\n",
                    CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X),
                    CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y));
    }

    delay(1000);
}
