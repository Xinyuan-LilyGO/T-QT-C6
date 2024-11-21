/*
 * @Description:
        This program is used to test the maximum power consumption of
    TQT-C6 during normal operation.
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditTime: 2024-07-11 09:32:44
 * @License: GPL 3.0
 */
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "xinyuandianzi";
const char *password = "AA15994823428";
// const char *ssid = "LilyGo-AABB";
// const char *password = "xinyuandianzi";

#define WIFI_CONNECT_WAIT_MAX 5000

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

// 文件下载链接
// const char *fileDownloadUrl = "https://code.visualstudio.com/docs/?dv=win64user";//vscode
// const char *fileDownloadUrl = "https://dldir1.qq.com/qqfile/qq/PCQQ9.7.17/QQ9.7.17.29225.exe"; // 腾讯CDN加速
// const char *fileDownloadUrl = "https://cd001.www.duba.net/duba/install/packages/ever/kinsthomeui_150_15.exe"; // 金山毒霸
const char *fileDownloadUrl = "http://music.163.com/song/media/outer/url?id=26122999.mp3";
// const char *fileDownloadUrl = "https://github.com/espressif/arduino-esp32/releases/download/3.0.1/esp32-3.0.1.zip";
// const char *fileDownloadUrl = "http://vipspeedtest8.wuhan.net.cn:8080/download?size=1073741824";

bool Wifi_Connection_State;
static size_t CycleTime = 0;

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

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816T->IIC_Interrupt_Flag = true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(BREATHING_LIGHT, OUTPUT);
    digitalWrite(BREATHING_LIGHT, LOW); // 开启呼吸灯
    // ledcAttach(BREATHING_LIGHT, 2000, 8);
    // ledcWrite(BREATHING_LIGHT, 0); // brightness 0 - 255
    // analogReadResolution(12);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, LOW); // 屏幕
    // ledcAttach(LCD_BL, 2000, 8);
    // ledcWrite(LCD_BL, 0); // brightness 0 - 255

    pinMode(BATTERY_ADC_DATA, INPUT_PULLDOWN);
    pinMode(BATTERY_MEASUREMENT_CONTROL, OUTPUT);
    digitalWrite(BATTERY_MEASUREMENT_CONTROL, LOW); // 开启电池电压测量

    if (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("CST816T initialization successfully");
    }

    if (ETA4662->begin() == false)
    {
        Serial.println("ETA4662 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("ETA4662 initialization successfully");
    }

    // ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
    //                                 ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON); // 充电
    // ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_WATCHDOG_MODE,
    //                                 ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);                 // 看门狗（当启动ETA4662的看门狗时，看门狗的定时器到达指定值后将断开电源重新连接，与ETA4662通信的MCU将重启）
    // ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_WATCHDOG_TIMER, 80); // 看门狗定时器值
    // ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_PCB_OTP_MODE,
    //                                 ETA4662->Ardu  ino_IIC_Power::Device_State::POWER_DEVICE_OFF);

    // 热调节阈值设置为120度
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 120);
    // 最小输入电压设置为3880mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_INPUT_VOLTAGE_LIMIT, 3880);
    // 充电目标电压电压设置为4215mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4215);
    // 系统电压设置为4600mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_SYSTEM_VOLTAGE_LIMIT, 4600);
    // 输入电流限制设置为500mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 500);
    // 快速充电电流限制设置为456mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 456);
    // 终端充电和预充电电流限制设置为5mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT, 5);
    // BAT到SYS的放电电流限制设置为2200mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT, 2200);

    if (LSM6DSL->begin() == false)
    {
        Serial.println("LSM6DSL initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("LSM6DSL initialization successfully");
    }

    // 设置加速度传感器性能模式为超高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_ACCELERATION_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_ULTRA_HIGH_POWER);
    // 设置加速度传感器灵敏度为 ±16g
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_SENSITIVITY,
                                    16);

    // 设置陀螺仪传感器性能模式为超高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_ULTRA_HIGH_POWER);
    // 设置陀螺仪传感器灵敏度为 ±2000dps
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_SENSITIVITY,
                                    2000);

    gfx->begin();
    gfx->fillScreen(WHITE);
    gfx->setTextColor(BLACK);

    // 连接到Wi-Fi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_17dBm);
    WiFi.begin(ssid, password);

    // WIFI_STA_Test_Loop();
}

void loop()
{
    gfx->fillScreen(WHITE);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting to xinyuandianzi...");

        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        gfx->fillScreen(WHITE);
        gfx->setCursor(0, 0);
        gfx->println("Connecting to xinyuandianzi...");
        gfx->printf("Running time: %ds\n\n", (uint32_t)millis() / 1000);
        delay(1000);
    }

    Serial.println("Successfully connected to xinyuandianzi");

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 0);
    gfx->println("Successfully connected to xinyuandianzi");

    // 初始化HTTP客户端
    HTTPClient http;
    http.begin(fileDownloadUrl);
    // 获取重定向的URL
    const char *headerKeys[] = {"Location"};
    http.collectHeaders(headerKeys, 1);

    // 记录下载开始时间
    size_t startTime = millis();
    // 无用时间
    size_t uselessTime = 0;

    // 发起GET请求
    int httpCode = http.GET();

    while (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND)
    {
        String newUrl = http.header("Location");
        Serial.printf("Redirecting to: %s\n", newUrl.c_str());
        // gfx->printf("Redirecting to: %s\n", newUrl.c_str());
        http.end(); // 关闭旧的HTTP连接

        // 使用新的URL重新发起GET请求
        http.begin(newUrl);
        httpCode = http.GET();
    }

    if (httpCode == HTTP_CODE_OK)
    {
        // 获取文件大小
        size_t fileSize = http.getSize();
        Serial.printf("Starting file download...\n");
        Serial.printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);
        gfx->printf("Starting file download...\n");
        gfx->printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);

        // 读取HTTP响应
        WiFiClient *stream = http.getStreamPtr();

        size_t temp_count_s = 0;
        size_t temp_fileSize = fileSize;
        uint8_t buf_1[4096] = {0};
        CycleTime = millis() + 1000; // 开始计时
        bool temp_count_flag = true;
        while (http.connected() && (temp_fileSize > 0 || temp_fileSize == -1))
        {
            // 获取可用数据的大小
            size_t availableSize = stream->available();
            if (availableSize)
            {
                temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)4096));

                if (millis() > CycleTime)
                {
                    size_t temp_time_1 = millis();
                    temp_count_s++;
                    Serial.printf("Download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                    Serial.printf("Remaining file size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);
                    Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

                    gfx->fillRect(0, 60, 128, 68, WHITE);
                    gfx->setCursor(0, 60);
                    gfx->printf("Speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                    gfx->printf("Size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);
                    gfx->printf("Running time: %ds\n\n", (uint32_t)millis() / 1000);

                    CycleTime = millis() + 1000;
                    size_t temp_time_2 = millis();

                    uselessTime = uselessTime + (temp_time_2 - temp_time_1);
                }
            }
            // delay(1);

            if (temp_count_s > 30)
            {
                temp_count_flag = false;
                break;
            }
        }

        // 关闭HTTP客户端
        http.end();

        // 记录下载结束时间并计算总花费时间
        size_t endTime = millis();

        gfx->fillRect(0, 60, 128, 68, WHITE);
        gfx->setCursor(0, 60);

        if (temp_count_flag == true)
        {
            Serial.printf("Download completed!\n");
            Serial.printf("Total download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            Serial.printf("Average download speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

            gfx->printf("Completed!\n");
            gfx->printf("Time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            gfx->printf("Speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
            gfx->printf("Running time: %ds\n\n", (uint32_t)millis() / 1000);
        }
        else
        {
            Serial.printf("Incomplete!\n");
            Serial.printf("Download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            Serial.printf("Average download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

            gfx->printf("Incomplete!\n");
            gfx->printf("Time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            gfx->printf("Speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
            gfx->printf("Running time: %ds\n\n", (uint32_t)millis() / 1000);
        }
    }
    else
    {
        Serial.printf("Failed to download\n");
        Serial.printf("Error httpCode: %d \n", httpCode);

        gfx->printf("Failed to download\n");
        gfx->printf("Error httpCode: %d \n\n", httpCode);
        gfx->printf("Running time: %ds\n\n", (uint32_t)millis() / 1000);
    }

    delay(5000);
}
