/*
 * @Description(CN): 
 *      这是针对于TQT-C6编写的用户测试程序CIT
 *
 * @Description(EN): 
 *      This is a user testing program CIT written for TQT-C6.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-22 11:59:37
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-20 14:12:30
 * @License: GPL 3.0
 */
#include "custom.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "events_init.h"

uint64_t Window_CycleTime1 = 0;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// N085-1212TBWIG06-C08
Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */); // Software SPI

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
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

lv_ui guider_ui;
Lvgl_CIT_UI CIT_UI;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if (CST816T->IIC_Interrupt_Flag == true)
    {
        CST816T->IIC_Interrupt_Flag = false;

        if (CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X) == 60 &&
            CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y) == 150)
        {
        }
        else
        {
            data->state = LV_INDEV_STATE_PR;

            /*Set the coordinates*/
            data->point.x = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            data->point.y = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

            // Serial.printf("Fingers Number:%d\n",
            //             CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER));
            // Serial.printf("Touch X:%d Y:%d\n",
            //             CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X),
            //             CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y));
        }
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void lvgl_initialization(void)
{
    lv_init();

    CIT_UI.LCD_Width = gfx->width();
    CIT_UI.LCD_Height = gfx->height();

    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * CIT_UI.LCD_Width * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    while (!disp_draw_buf)
    {
        Serial.println("LVGL disp_draw_buf allocate failed!");
        delay(1000);
    }

    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, CIT_UI.LCD_Width * 40);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = CIT_UI.LCD_Width;
    disp_drv.ver_res = CIT_UI.LCD_Height;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
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

    while (ETA4662->begin() == false)
    {
        Serial.println("ETA4662 initialization fail");
        delay(2000);
    }
    Serial.println("ETA4662 initialization successfully");

    //  注意事项：当ETA4662开启充电，但是又未接入电池，此时ETA4662会将输出电压断开一次，
    // 与ETA4662通信的MCU将失去电源重启，所以如果没有接入电池时请不要开启ETA4662的充电
    // ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
    //                                 ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON); // 充电

    // 热调节阈值设置为60度
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 60);
    // 最小输入电压设置为4760mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_INPUT_VOLTAGE_LIMIT, 4760);
    // 充电目标电压电压设置为4215mV
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4215);
    // 系统电压设置为4950mV（输出电压）
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_SYSTEM_VOLTAGE_LIMIT, 4950);
    // 输入电流限制设置为470mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 470);
    // 快速充电电流限制设置为136mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 456);
    // 终端充电和预充电电流限制设置为5mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT, 5);
    // BAT到SYS的放电电流限制设置为2200mA
    ETA4662->IIC_Write_Device_Value(ETA4662->Arduino_IIC_Power::Device_Value::POWER_DEVICE_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT, 2200);

    while (CST816T->begin() == false)
    {
        Serial.println("CST816T initialization fail");
        delay(2000);
    }
    Serial.println("CST816T initialization successfully");

    // 检测到触摸状态变化时，发出低脉冲
    CST816T->IIC_Write_Device_State(CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816T->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

    gfx->begin();
    gfx->fillScreen(BLACK);

    lvgl_initialization();

    setup_ui(&guider_ui);
    events_init(&guider_ui);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    // delay(5);

    switch (CIT_UI.Window_Current_State)
    {
    case CIT_UI.Window_Current_State::Window_LCD_Backlight_Test: // LCD_Backlight_Test
        if (CIT_UI.Window_Initialization_Flag == false)          // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Btn_Start_Testing_Initialization(&guider_ui);
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_LCD_Backlight_Test_Loop();
                Window_CycleTime1 = millis() + 1; // 1ms
            }
        }
        break;
    case CIT_UI.Window_Current_State::Window_LCD_Display_Color_Test: // LCD_Display_Color_Test
        if (CIT_UI.Window_Initialization_Flag == false)              // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Btn_Start_Testing_Initialization(&guider_ui);
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_LCD_Display_Color_Test_Loop();
                Window_CycleTime1 = millis() + 1; // 1ms
            }
        }
        break;
    case CIT_UI.Window_Current_State::Window_Touch_Test: // Touch_Test
        if (CIT_UI.Window_Initialization_Flag == false)  // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Tabview1_Test_Initialization(&guider_ui);
            Window_Touch_Test_Initialization(&guider_ui);
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_Touch_Test_Loop();
                Window_CycleTime1 = millis() + 500; // 500ms
            }
        }
        break;
    case CIT_UI.Window_Current_State::Window_Power_Test: // Power_Test
        if (CIT_UI.Window_Initialization_Flag == false)  // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Tabview2_Test_Initialization(&guider_ui);
            Window_Power_Test_Initialization(&guider_ui);
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_Power_Test_Loop();
                Window_CycleTime1 = millis() + 500; // 500ms
            }
        }
        break;
    case CIT_UI.Window_Current_State::Window_Breathing_Light_Test: // Breathing_Light_Test
        if (CIT_UI.Window_Initialization_Flag == false)            // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Slider_Test_Initialization(&guider_ui);
            Window_Breathing_Light_Test_Initialization(&guider_ui);
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_Breathing_Light_Test_Loop();
                Window_CycleTime1 = millis() + 3; // 3ms
            }
        }
        break;
    case CIT_UI.Window_Current_State::Window_WIFI_STA_Test: // WIFI_STA_Test
        if (CIT_UI.Window_Initialization_Flag == false)     // 初始化
        {
            CIT_UI.Window_Initialization_Flag = true;
            Tabview1_Test_Initialization(&guider_ui);
            Window_WIFI_STA_Test_Initialization(&guider_ui);

            Window_CycleTime1 = millis() + 500; // 500ms
            CIT_UI.Window_WIFI_STA_Test_State = true;
        }
        else
        {
            if (millis() > Window_CycleTime1)
            {
                Window_WIFI_STA_Test_Loop();
                Window_CycleTime1 = millis() + 3; // 3ms
            }
        }
        break;

    default:
        break;
    }
}