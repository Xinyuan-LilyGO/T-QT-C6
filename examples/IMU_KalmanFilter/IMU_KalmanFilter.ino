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
 * @LastEditTime: 2024-07-11 14:03:38
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include <math.h>
#include "pin_config.h"
#include "Kalman.h"

static size_t CycleTime1 = 0;
static size_t CycleTime2 = 0;
static size_t timer;

double roll, pitch;
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY;   // Calculated angle using a Kalman filter

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;

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

    while (LSM6DSL->begin() == false)
    {
        Serial.println("LSM6DSL initialization fail");
        delay(2000);
    }
    Serial.println("LSM6DSL initialization successfully");

    // 设置加速度传感器模式为高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_ACCELERATION_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_NORMAL_POWER);
    // 设置加速度传感器灵敏度为 ±16g
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_SENSITIVITY,
                                    16);

    // 设置陀螺仪传感器模式为高性能模式
    LSM6DSL->IIC_Write_Device_State(LSM6DSL->Arduino_IIC_IMU::Device::IMU_GYROSCOPE_POWER_MODE,
                                    LSM6DSL->Arduino_IIC_IMU::Device_Mode::IMU_DEVICE_NORMAL_POWER);
    // 设置陀螺仪传感器灵敏度为 ±2000dps
    LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_SENSITIVITY,
                                    2000);

    Serial.printf("\nStart calibrating the gyroscope and accelerometer\n");
    delay(1000);
    // 将器件静止后再校正陀螺仪传感器
    while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_CORRECTION,
                                           100) == false)
    {
        Serial.printf("\nLSM6DSL gyroscope correction fail\n");
        Serial.printf("Please ensure that the device is in a stationary state!\n\n");
        delay(1000);
    }
    Serial.printf("LSM6DSL gyroscope correction successfully\n");
    // 将器件正放静止后再校正加速度传感器
    // 最好在陀螺仪校正后立即进行加速度校正
    while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_CORRECTION,
                                           100) == false)
    {
        Serial.printf("\nLSM6DSL acceleration correction fail\n");
        Serial.printf("Please ensure that the device is in a stationary state!\n\n");
        delay(1000);
    }
    Serial.printf("LSM6DSL acceleration correction successfully\n\n");
    delay(1000);
}
void loop()
{
    // if (millis() > CycleTime1)
    // {
    //     Serial.printf("--------------------LSM6DSL--------------------\n");
    //     Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);
    //     Serial.printf("IIC_Bus.use_count(): %d\n\n", (int32_t)IIC_Bus.use_count());

    //     Serial.printf("ID: %#X \n", (int32_t)LSM6DSL->IIC_Read_Device_ID());

    //     // 只有在启动加速度或陀螺仪的时候才能查看设备温度
    //     Serial.printf("IMU Device Temperature: %.3f ^C \n",
    //                   LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_TEMPERATURE_VALUE));

    //     Serial.printf("--------------------LSM6DSL--------------------\n");
    //     CycleTime1 = millis() + 5000;
    // }

    if (millis() > CycleTime2)
    {
        // Arduino-IDE Serial Plotter
        // IMU Acceleration
        // FIFO OFF
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

        // FIFO ON

        // IMU Gyroscope
        // FIFO OFF
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
        // Serial.printf("%.6f,%.6f", (float)-10, (float)10);
        // Serial.printf(",%.6f",
        //               (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_X_SIGNED) / 1000);
        // delay(5);
        // Serial.printf(",%.6f",
        //               (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Y_SIGNED) / 1000);
        // delay(5);
        // Serial.printf(",%.6f\n",
        //               (acos(-1) / 180) * LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Z_SIGNED) / 1000);
        // delay(5);

        // FIFO ON

        // Serial.printf("\n\n");

        // KalmanFilter
        accX = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_X_SIGNED);
        delay(5);
        accY = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Y_SIGNED);
        delay(5);
        accZ = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_ACCELERATION_Z_SIGNED);
        delay(5);
        gyroX = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_X_SIGNED);
        delay(5);
        gyroY = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Y_SIGNED);
        delay(5);
        gyroZ = LSM6DSL->IIC_Read_Device_Value(LSM6DSL->Arduino_IIC_IMU::Value_Information::IMU_GYROSCOPE_Z_SIGNED);
        delay(5);

        double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
        timer = micros();

        roll = atan2(accY, accZ) * RAD_TO_DEG;
        pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;

        double gyroXrate = gyroX / 131.0; // Convert to deg/s
        double gyroYrate = gyroY / 131.0; // Convert to deg/s

        // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
        if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90))
        {
            kalmanX.setAngle(roll);
            compAngleX = roll;
            kalAngleX = roll;
            gyroXangle = roll;
        }
        else
            kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

        if (abs(kalAngleX) > 90)
            gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
        kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);

        gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
        gyroYangle += gyroYrate * dt;
        // gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
        // gyroYangle += kalmanY.getRate() * dt;

        compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
        compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

        // Reset the gyro angle when it has drifted too much
        if (gyroXangle < -180 || gyroXangle > 180)
            gyroXangle = kalAngleX;
        if (gyroYangle < -180 || gyroYangle > 180)
            gyroYangle = kalAngleY;

        Serial.printf("%.6f,%.6f,%.6f,%.6f\n", (float)-200, (float)200, roll, pitch);

        // Serial.printf("%.6f,%.6f,%.6f,%.6f,%.6f\n", (float)-200, (float)200, gyroXangle, compAngleX, kalAngleX);
        // Serial.printf("%.6f,%.6f,%.6f,%.6f,%.6f\n", (float)-200, (float)200, gyroYangle, compAngleY, kalAngleY);

        CycleTime2 = millis() + 10;
    }
}
