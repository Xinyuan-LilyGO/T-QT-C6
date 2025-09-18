/*
 *  @Description:
            LSM6DSL chip routine based on Arduino_DriveBus library
        Here are a few points to note when using this chip:
        1.When collecting data in high-performance mode, LSM6DSL may experience significant data fluctuations.
        2.When directly reading data from LSM6DSL, it is recommended to introduce a delay in the read rate in
    low-performance mode to avoid data reading failures.
        3.Calibration is required when using LSM6DSL. During calibration, ensure the device is placed upright
    and stationary. The calibration sequence should start from the gyroscope sensor to the accelerometer sensor.
 * @Author: LILYGO_L
 * @Date: 2024-02-20 17:13:03
 * @LastEditTime: 2025-06-03 11:42:27
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include <math.h>
#include "pin_config.h"

static size_t CycleTime1 = 0;
static size_t CycleTime2 = 0;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> LSM6DSL(new Arduino_LSM6DSL(IIC_Bus, LSM6DSL_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> SGM41562(new Arduino_SGM41562(IIC_Bus, SGM41562_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

#if defined T_QT_C6_Battery_V1_0_V1_1
    pinMode(LSM6DSL_IIC_ADDRESS_MODE, OUTPUT);
    digitalWrite(LSM6DSL_IIC_ADDRESS_MODE, LOW); // 设置LSM6DSL的IIC地址为0x6A
#elif defined T_QT_C6_Battery_V1_2
    pinMode(LSM6DSL_EN, OUTPUT);
    digitalWrite(LSM6DSL_EN, HIGH); // 开启LSM6DSL
#endif

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

    while (LSM6DSL->begin() == false)
    {
        Serial.println("LSM6DSL initialization fail");
        delay(2000);
    }
    Serial.println("LSM6DSL initialization successfully");

    // 设置加速度传感器性能模式为高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_ACCELERATION_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_HIGH_POWER);
    // 设置加速度传感器灵敏度为 ±16g
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_SENSITIVITY,
                                    16);

    // 设置陀螺仪传感器性能模式为高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_HIGH_POWER);
    // 设置陀螺仪传感器灵敏度为 ±2000dps
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_SENSITIVITY,
                                    2000);

    // Serial.printf("\nStart calibrating the gyroscope and accelerometer\n");
    // delay(1000);
    // // 将器件静止后再校正陀螺仪传感器
    // while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_CORRECTION,
    //                                        100) == false)
    // {
    //     Serial.printf("\nLSM6DSL gyroscope correction fail\n");
    //     Serial.printf("Please ensure that the device is in a stationary state!\n\n");
    //     delay(1000);
    // }
    // Serial.printf("LSM6DSL gyroscope correction successfully\n");
    // // 将器件正放静止后再校正加速度传感器
    // // 最好在陀螺仪校正后立即进行加速度校正
    // while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_CORRECTION,
    //                                        100) == false)
    // {
    //     Serial.printf("\nLSM6DSL acceleration correction fail\n");
    //     Serial.printf("Please ensure that the device is in a stationary state!\n\n");
    //     delay(1000);
    // }
    // Serial.printf("LSM6DSL acceleration correction successfully\n\n");
    // delay(1000);

    // FIFO ON
    // //  设置设备FIFO性能模式为高性能模式
    // LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_FIFO_POWER_MODE,
    //                                 LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_HIGH_POWER);
    // // 设置设备FIFO工作模式为Bypass模式
    // LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_FIFO_WORK_MODE,
    //                                 LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_FIFO_BYPASS);
    // // 设置设备FIFO获取数据类型为加速度和陀螺仪传感器的数据
    // LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_FIFO_DATA_TYPE,
    //                                 LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_FIFO_DATA_ACCELERATION_GYROSCOPE);
    // // 设置加速度传感器的FIFO的抽取因子为1
    // LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_FIFO_ACCELERATION_FACTOR,
    //                                 1);
    // // 设置陀螺仪传感器的FIFO的抽取因子为1
    // LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_FIFO_GYROSCOPE_FACTOR,
    //                                 1);
}
void loop()
{
    // if (millis() > CycleTime1)
    // {
    //     Serial.printf("--------------------LSM6DSL--------------------\n");
    //     Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);
    //     Serial.printf("IIC_Bus.use_count(): %d\n\n", (int32_t)IIC_Bus.use_count());

    //     Serial.printf("ID: %#X \n", (int32_t)LSM6DSL->IIC_Device_ID());

    //     // 只有在启动加速度或陀螺仪的时候才能查看设备温度
    //     Serial.printf("IMU Device Temperature: %.3f ^C \n",
    //                   LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_TEMPERATURE_VALUE));

    //     Serial.printf("--------------------LSM6DSL--------------------\n");
    //     CycleTime1 = millis() + 5000;
    // }

    if (millis() > CycleTime2)
    {
        // FIFO OFF
        // Arduino-IDE Serial Plotter
        // IMU Acceleration
        // 单位mg
        // Serial.printf("%.6f,%.6f", (float)-1200, (float)1200);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_X_SIGNED));
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Y_SIGNED));
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Z_SIGNED));
        // delay(5);
        // Serial.printf("\n");

        // g单位转化为m/s²单位
        // 将加速度计（g）的值乘以9.81即可将其转换为米每秒的平方
        // Serial.printf("%.6f,%.6f", (float)-12, (float)12);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_X_SIGNED) / 1000 * 9.81);
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Y_SIGNED) / 1000 * 9.81);
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Z_SIGNED) / 1000 * 9.81);
        // delay(5);
        // Serial.printf("\n");

        // IMU Gyroscope
        // 单位dps/s
        // Serial.printf("%.6f,%.6f", (float)-2000, (float)2000);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_X_SIGNED) / 1000);
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Y_SIGNED) / 1000);
        // delay(5);
        // Serial.printf(",%.6f",
        //               LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Z_SIGNED) / 1000);
        // delay(5);
        // Serial.printf("\n");

        // dps/s单位转化为rad/s单位
        // 一圈等于360度或2π弧度 每秒转的度数需要乘以 π/180 来转换为弧度每秒
        Serial.printf("%.6f,%.6f", (float)-10, (float)10);
        Serial.printf(",%.6f",
                      (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_X_SIGNED) / 1000);
        delay(10);
        Serial.printf(",%.6f",
                      (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Y_SIGNED) / 1000);
        delay(10);
        Serial.printf(",%.6f\n",
                      (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Z_SIGNED) / 1000);
        delay(10);

        // FIFO ON
        // // 重启FIFO清除FIFO缓存中的数据
        // LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_FIFO_WORK_MODE,
        //                                 LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_FIFO_BYPASS);
        // LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_FIFO_WORK_MODE,
        //                                 LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_FIFO_NORMAL);

        // // DATA_PATTERN 范围为0-1023
        // Serial.printf("%d ",
        //               (int16_t)LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_FIFO_DATA_PATTERN));
        // for (int i = 0; i < 1024; i++)
        // {
        //     Serial.printf("%d ",
        //                   (int16_t)LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_FIFO_DATA));
        // }
        // Serial.printf("\n");

        CycleTime2 = millis() + 50;
    }
}
