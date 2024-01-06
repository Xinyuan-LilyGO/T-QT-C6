/*
 * @Description(CN): 
 *      该程序用于测试TQT-C6正常工作时的最低功耗
 *
 * @Description(EN): 
 *      This program is used to test the minimum power consumption of 
 *  TQT-C6 during normal operation.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-01-06 14:18:19
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
    pinMode(Breathing_Light, OUTPUT);
    digitalWrite(Breathing_Light, HIGH); // 关闭呼吸灯
    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 255); // brightness 0 - 255
    analogReadResolution(12);

    ledcAttach(LCD_BL, 20000, 8);
    ledcWrite(LCD_BL, 0); // brightness 0 - 255

    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量

    while (CST816T->begin() == false)
    {
        delay(2000);
    }

    while (ETA4662->begin() == false)
    {
        delay(2000);
    }

    ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                    ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF); // 充电
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
    gfx->fillScreen(WHITE);

    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(MAGENTA);
    gfx->println("Enter Low-Power-Dissipation in 3 seconds");

    delay(3000);

    ledcWrite(LCD_BL, 255); // brightness 0 - 255
}

void loop()
{
    delay(1000);
}
