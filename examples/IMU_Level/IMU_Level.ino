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
 * @LastEditTime: 2024-07-11 15:12:03
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "Arduino_GFX_Library.h"
#include <math.h>
#include "pin_config.h"
#include "Kalman.h"

static size_t CycleTime1 = 0;
static size_t CycleTime2 = 0;
static size_t CycleTime3 = 0;
static size_t timer;

int32_t BREATHING_LIGHT_Brightness = 0;
bool Battery_Charging_Flag = false;
bool BREATHING_LIGHT_State = false;

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

// N085-1212TBWIG06-C08
Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    2 /* col offset 1 */, 1 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

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
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 2000, 8);
    ledcWrite(LCD_BL, 0); // 开启屏幕

    gfx->begin();
    gfx->fillScreen(WHITE);

    while (LSM6DSL->begin() == false)
    {
        Serial.println("LSM6DSL initialization fail");
        gfx->fillScreen(WHITE);
        gfx->setCursor(10, 64);
        gfx->println("LSM6DSL initialization fail");
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
    gfx->setCursor(10, 60);
    gfx->setTextColor(RED);
    gfx->printf("Start calibrating the gyroscope and accelerometer");
    delay(1000);
    // 将器件静止后再校正陀螺仪传感器
    while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_GYROSCOPE_CORRECTION,
                                           100) == false)
    {
        Serial.printf("\nLSM6DSL gyroscope correction fail\n");
        Serial.printf("Please ensure that the device is in a stationary state!\n\n");

        gfx->fillScreen(WHITE);
        gfx->setCursor(10, 60);
        gfx->setTextColor(RED);
        gfx->printf("LSM6DSL gyroscope correction fail\nPlease ensure that the device is in a stationary state!");
        delay(1000);
    }
    Serial.printf("LSM6DSL gyroscope correction successfully\n");
    gfx->fillScreen(WHITE);
    gfx->setCursor(10, 60);
    gfx->setTextColor(RED);
    gfx->printf("LSM6DSL gyroscope correction successfully");
    // 将器件正放静止后再校正加速度传感器
    // 最好在陀螺仪校正后立即进行加速度校正
    while (LSM6DSL->IIC_Write_Device_Value(LSM6DSL->Arduino_IIC_IMU::Device_Value::IMU_ACCELERATION_CORRECTION,
                                           100) == false)
    {
        Serial.printf("\nLSM6DSL acceleration correction fail\n");
        Serial.printf("Please ensure that the device is in a stationary state!\n\n");
        gfx->fillScreen(WHITE);
        gfx->setCursor(10, 60);
        gfx->setTextColor(RED);
        gfx->printf("LSM6DSL acceleration correction fail\nPlease ensure that the device is in a stationary state!");
        delay(1000);
    }
    Serial.printf("LSM6DSL acceleration correction successfully\n\n");
    gfx->fillScreen(WHITE);
    gfx->setCursor(10, 60);
    gfx->setTextColor(RED);
    gfx->printf("LSM6DSL acceleration correction successfully");
    delay(1000);

    gfx->fillScreen(WHITE);
    gfx->drawRect(14, 0, 100, 100, RED);
    gfx->fillCircle(64, 50, 3, RED);
}
void loop()
{
    if (millis() > CycleTime1)
    {
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

        double roll_2, pitch_2;
        if ((roll > 90))
        {
            roll = 90 - (roll - 90);
        }
        if ((roll < (-90)))
        {
            roll = -90 - (roll + 90);
        }
        gfx->fillRect(14 + 1, 0 + 1, 100 - 2, 100 - 2, WHITE);
        gfx->fillCircle(64, 50, 2, RED);
        gfx->drawCircle(64 + (pitch * ((50.0 - 5.0 - 1.0) / 90.0)),
                        50 + (roll * ((50.0 - 5.0 - 1.0) / 90.0)), 5, ORANGE);

        CycleTime1 = millis() + 10;
    }

    if (millis() > CycleTime2)
    {
        Serial.printf("%.6f,%.6f,%.6f,%.6f\n", (float)-200, (float)200, roll, pitch);
        gfx->fillRect(0, 102, 128, 26, WHITE);
        gfx->setTextColor(RED);

        gfx->setCursor(22, 107);
        gfx->printf("%.3f , %.3f", roll, pitch);

        CycleTime2 = millis() + 100;
    }

}
