/*
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-10-06 10:54:55
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-11 09:16:01
 * @License: GPL 3.0
 */
#pragma once

#include "gui_guider.h"
#include "Arduino_DriveBus_Library.h"
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

#define LV_LVGL_H_INCLUDE_SIMPLE
#define LV_USE_DCLOCK
#define LV_USE_CAROUSEL

class Lvgl_CIT_UI
{
public:
    enum Window_Current_State
    {
        Window_NULL = 0,
        Window_Home,
        Window_LCD_Backlight_Test,
        Window_LCD_Display_Color_Test,
        Window_Touch_Test,
        Window_Power_Test,
        Window_Breathing_Light_Test,
        Window_WIFI_STA_Test,
    };
    enum Window_Test_Result
    {
        Test_NULL = 0,
        Test_Pass,
        Test_Fail,
    };

    // Window
    uint8_t Window_Current_State = Window_Current_State::Window_NULL; // 当前正在运行的窗口
    bool Window_Initialization_Flag = false;
    bool Window_Button_Start_Testing_Flag = false;

    // Result
    uint8_t Window_LCD_Backlight_Test_Result = Window_Test_Result::Test_NULL;
    uint8_t Window_LCD_Display_Color_Test_Result = Window_Test_Result::Test_NULL;
    uint8_t Window_Touch_Test_Result = Window_Test_Result::Test_NULL;
    uint8_t Window_Power_Test_Result = Window_Test_Result::Test_NULL;
    uint8_t Window_Breathing_Light_Test_Result = Window_Test_Result::Test_NULL;
    uint8_t Window_WIFI_STA_Test_Result = Window_Test_Result::Test_NULL;

    // WIFI
    uint8_t Window_WIFI_STA_Test_State = false;

    bool Breathing_Light_State = false;
    int32_t Breathing_Light_Brightness = -1;

    // LCD
    uint32_t LCD_Width = 0;
    uint32_t LCD_Height = 0;
};

void Btn_Start_Testing_Initialization(lv_ui *ui);
void Tabview1_Test_Initialization(lv_ui *ui);
void Tabview2_Test_Initialization(lv_ui *ui);
void Slider_Test_Initialization(lv_ui *ui);
void Window_Touch_Test_Initialization(lv_ui *ui);
void Window_Power_Test_Initialization(lv_ui *ui);
void Window_Breathing_Light_Test_Initialization(lv_ui *ui);
void Window_WIFI_STA_Test_Initialization(lv_ui *ui);
void Window_LCD_Backlight_Test_Loop(void);
void Window_LCD_Display_Color_Test_Loop(void);
void Window_Touch_Test_Loop(void);
void Window_Power_Test_Loop(void);
void Window_Breathing_Light_Test_Loop(void);
void Window_WIFI_STA_Test_Loop(void);

void custom_init(lv_ui *ui);

extern Lvgl_CIT_UI CIT_UI;
extern Arduino_GFX *gfx;
extern std::unique_ptr<Arduino_IIC> CST816T;
extern std::unique_ptr<Arduino_IIC> ETA4662;
