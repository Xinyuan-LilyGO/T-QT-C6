/*
 * @Description: TQT-C6触摸测试程序
 *      默认开启CST816自动睡眠模式，只有在触摸条件发生的时候芯片才能被读数据和写数据
 * 
 * @Description: TQT-C6 Touch Test Program
 *      Default enables CST816 automatic sleep mode, only when touch conditions occur 
 *  can the chip be read and written data.
 * 
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-25 17:09:20
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 14:15:11
 * @License: GPL 3.0
 */
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    2 /* col offset 1 */, 1 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(IIC_Bus, CST816T_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816T->IIC_Interrupt_Flag = true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    // 呼吸灯
    pinMode(Breathing_Light, OUTPUT);
    ledcAttach(Breathing_Light, 20000, 8);
    ledcWrite(Breathing_Light, 255); // 关闭呼吸灯

    // 测量电池
    pinMode(Battery_ADC_Data, INPUT_PULLDOWN);
    pinMode(Battery_Measurement_Control, OUTPUT);
    digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量
    analogReadResolution(12);

    // 屏幕背光
    pinMode(LCD_BL, OUTPUT);
    ledcAttach(LCD_BL, 20000, 8);
    ledcWrite(LCD_BL, 0); // 开启屏幕

    while (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    Serial.println("CST816T initialization successfully");

    // 中断模式为检测到手势时，发出低脉冲
    //  CST816T->IIC_Write_Device_State(CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
    //                                  CST816T->Arduino_IIC_Touch::Device_Mode::TOUCH_INTERRUPT_MOTION);

    // 目前休眠功能只能进入不能退出 所有不建议开启休眠
    // CST816T->IIC_Write_Device_State(CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_SLEEP_MODE,
    //                                 CST816T->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setTextColor(PINK);
    gfx->setCursor(0, 128 / 2);

    while ((int32_t)CST816T->IIC_Read_Device_ID() == -1) // 等待读取到ID
    {
        Serial.println("ID read failed");
        delay(1000);
    }

    gfx->printf("ID: %#X \n\n", (int32_t)CST816T->IIC_Read_Device_ID());
    gfx->setTextColor(MAGENTA);
    delay(1000);
}

void loop()
{
    Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    if (CST816T->IIC_Interrupt_Flag == true)
    {
        CST816T->IIC_Interrupt_Flag = false;

        gfx->setCursor(50, 50);
        gfx->fillScreen(WHITE);

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
