/*
 * @Description:
            SGM41562 Example Program
        When enabling the SGM41562 watchdog, the MCU communicating with SGM41562 will
    lose power and restart when the watchdog timer reaches the specified value, as the power
    supply will be disconnected and reconnected.
 * @Author: LILYGO_L
 * @Date: 2023-11-27 10:08:51
 * @LastEditTime: 2023-12-20 11:55:28
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    // 呼吸灯
    pinMode(BREATHING_LIGHT, OUTPUT);
    ledcAttach(BREATHING_LIGHT, 2000, 8);
    ledcWrite(BREATHING_LIGHT, 255); // 关闭呼吸灯

    // 测量电池
    pinMode(BATTERY_ADC_DATA, INPUT_PULLDOWN);
    pinMode(BATTERY_MEASUREMENT_CONTROL, OUTPUT);
    digitalWrite(BATTERY_MEASUREMENT_CONTROL, LOW); // 开启电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 2000, 8);
    ledcWrite(LCD_BL, 255); // 关闭屏幕

    while (SGM41562->begin() == false)
    {
        Serial.println("SGM41562 initialization fail");
        delay(2000);
    }
    Serial.println("SGM41562 initialization successfully");

    // SGM41562->IIC_Write_Device_State(SGM41562->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
    //                                 SGM41562->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON); // 充电
    // SGM41562->IIC_Write_Device_State(SGM41562->Arduino_IIC_Power::Device::POWER_DEVICE_WATCHDOG_MODE,
    //                                 SGM41562->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);                 // 看门狗（当启动SGM41562的看门狗时，看门狗的定时器到达指定值后将断开电源重新连接，与SGM41562通信的MCU将重启）
    // SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_WATCHDOG_TIMER, 80); // 看门狗定时器值

    // 热调节阈值设置为60度
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 60);
    // 最小输入电压设置为3880mV
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_INPUT_VOLTAGE_LIMIT, 3880);
    // 充电目标电压电压设置为4215mV
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4215);
    // 系统电压设置为4600mV
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_SYSTEM_VOLTAGE_LIMIT, 4600);
    // 输入电流限制设置为500mA
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 500);
    // 快速充电电流限制设置为456mA
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 456);
    // 终端充电和预充电电流限制设置为5mA
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT, 5);
    // BAT到SYS的放电电流限制设置为2200mA
    SGM41562->IIC_Write_Device_Value(SGM41562->Arduino_IIC_Power::Device_Value::POWER_DEVICE_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT, 2200);
}
void loop()
{
    String Battery_Status = SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_BATTERY_FAULT_STATUS);

    Serial.printf("--------------------SGM41562--------------------\n");
    Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);
    Serial.printf("IIC_Bus.use_count(): %d\n\n", (int32_t)IIC_Bus.use_count());

    Serial.printf("ID: %#X \n", (int32_t)SGM41562->IIC_Read_Device_ID());

    Serial.printf("\nCharging Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_CHARGING_STATUS)).c_str());
    Serial.printf("Input Source Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_INPUT_SOURCE_STATUS)).c_str());
    Serial.printf("System Voltage Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_SYSTEM_VOLTAGE_STATUS)).c_str());
    Serial.printf("Thermal Regulation Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_THERMAL_REGULATION_STATUS)).c_str());

    Serial.printf("\nWatchdog Fault Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_WATCHDOG_FAULT_STATUS)).c_str());
    Serial.printf("Input Fault Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_INPUT_FAULT_STATUS)).c_str());
    Serial.printf("Thermal Shutdown Fault Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_THERMAL_SHUTDOWN_FAULT_STATUS)).c_str());
    Serial.printf("Battery Fault Status: %s \n", (Battery_Status).c_str());
    Serial.printf("Safety Timer Fault Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_SAFETY_TIMER_STATUS_FAULT_STATUS)).c_str());
    Serial.printf("NTC Fault Status: %s \n",
                  (SGM41562->IIC_Read_Device_State(SGM41562->Arduino_IIC_Power::Status_Information::POWER_NTC_FAULT_STATUS)).c_str());

    Serial.printf("\nThermal Regulation Threshold: %d ^C \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_THERMAL_REGULATION_THRESHOLD));

    if (Battery_Status == "Normal") // 开启充电的时候可检测该值从而推断电池是否接入
    {
        Serial.printf("\nBattery Voltage: %d mV\n", analogReadMilliVolts(BATTERY_ADC_DATA) * 2);
    }
    else
    {
        Serial.printf("\nBattery Voltage: 0 mV\n");
    }

    Serial.printf("\nInput Minimum Voltage Limit: %d mV \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_MINIMUM_INPUT_VOLTAGE_LIMIT));
    Serial.printf("Charging Target Voltage Limit: %d mV \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_CHARGING_TARGET_VOLTAGE_LIMIT));
    Serial.printf("System Voltage Limit: %d mV \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_SYSTEM_VOLTAGE_LIMIT));
    Serial.printf("Input Current Limit: %d mA \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_INPUT_CURRENT_LIMIT));
    Serial.printf("Fast Charge Current Limit: %d mA \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_FAST_CHARGING_CURRENT_LIMIT));
    Serial.printf("Termination And Precondition Charge Current Limit: %d mA \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT));
    Serial.printf("BAT To SYS Discharge Current Limit: %d mA \n",
                  (int32_t)SGM41562->IIC_Read_Device_Value(SGM41562->Arduino_IIC_Power::Value_Information::POWER_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT));

    Serial.printf("--------------------SGM41562--------------------\n");

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(5);
    }
    for (int i = 0; i <= 255; i++)
    {
        ledcWrite(BREATHING_LIGHT, i);
        delay(2);
    }

    delay(1000);
}
