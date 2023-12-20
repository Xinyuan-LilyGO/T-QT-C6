/*
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-10-05 11:31:11
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-11 11:48:25
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include <stdio.h>
#include "lvgl.h"
#include "widgets_init.h"
#include "custom.h"
#include "pin_config.h"
#include "events_init.h"
#include "Material_16Bit.h"
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

void Btn_Start_Testing_Initialization(lv_ui *ui)
{
    // Write codes Test_btn_1
    ui->Test_btn_1 = lv_btn_create(ui->Test);
    ui->Test_btn_1_label = lv_label_create(ui->Test_btn_1);
    lv_label_set_text(ui->Test_btn_1_label, "Start testing");
    lv_label_set_long_mode(ui->Test_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->Test_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->Test_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->Test_btn_1, CIT_UI.LCD_Width / 9.1, CIT_UI.LCD_Height / 3.2);
    lv_obj_set_size(ui->Test_btn_1, CIT_UI.LCD_Width / 1.28, CIT_UI.LCD_Height / 4.2);
    lv_obj_set_scrollbar_mode(ui->Test_btn_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_btn_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_btn_1, lv_color_hex(0xff7f58), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Test_btn_1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->Test_btn_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->Test_btn_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_btn_1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_btn_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Test_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Test_btn_1, &lv_font_arial_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Test_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);

    // Init events for screen.
    events_init_Test(ui);
}

void Slider_Test_Initialization(lv_ui *ui)
{
    // Write codes Test_label_2
    ui->Test_label_2 = lv_label_create(ui->Test);
    lv_label_set_text(ui->Test_label_2, "");
    lv_label_set_long_mode(ui->Test_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->Test_label_2, 0, 35);
    lv_obj_set_size(ui->Test_label_2, 128, 20);
    lv_obj_set_scrollbar_mode(ui->Test_label_2, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Test_label_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Test_label_2, &lv_font_arial_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Test_label_2, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Test_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes Test_slider_1
    ui->Test_slider_1 = lv_slider_create(ui->Test);
    lv_slider_set_range(ui->Test_slider_1, 0, 255);
    lv_slider_set_value(ui->Test_slider_1, 0, lv_anim_enable_t::LV_ANIM_OFF);
    lv_obj_set_pos(ui->Test_slider_1, 11, 62);
    lv_obj_set_size(ui->Test_slider_1, 107, 10);
    lv_obj_set_scrollbar_mode(ui->Test_slider_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_slider_1, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->Test_slider_1, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->Test_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_slider_1, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->Test_slider_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_slider_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for Test_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_slider_1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_slider_1, lv_color_hex(0x5691f8), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->Test_slider_1, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->Test_slider_1, lv_color_hex(0xa666f1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_slider_1, 44, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style for Test_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_slider_1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_slider_1, 44, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Tabview1_Test_Initialization(lv_ui *ui)
{
    // Write codes Test_tabview_1
    ui->Test_tabview_1 = lv_tabview_create(ui->Test, LV_DIR_TOP, CIT_UI.LCD_Height / 5.12);
    lv_obj_set_pos(ui->Test_tabview_1, 0, 0);
    lv_obj_set_size(ui->Test_tabview_1, CIT_UI.LCD_Width, CIT_UI.LCD_Height - (CIT_UI.LCD_Height / 5.12));
    lv_obj_set_scrollbar_mode(ui->Test_tabview_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_tabview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_tabview_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_tabview_1, lv_color_hex(0xeaeff3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Test_tabview_1, lv_color_hex(0x4d4d4d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Test_tabview_1, &lv_font_arial_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Test_tabview_1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for &style_Test_tabview_1_extra_btnm_main_default
    static lv_style_t style_Test_tabview_1_extra_btnm_main_default;
    ui_init_style(&style_Test_tabview_1_extra_btnm_main_default);

    lv_style_set_bg_opa(&style_Test_tabview_1_extra_btnm_main_default, 255);
    lv_style_set_bg_color(&style_Test_tabview_1_extra_btnm_main_default, lv_color_hex(0xffffff));
    lv_style_set_border_width(&style_Test_tabview_1_extra_btnm_main_default, 0);
    lv_style_set_radius(&style_Test_tabview_1_extra_btnm_main_default, 0);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for &style_Test_tabview_1_extra_btnm_items_default
    static lv_style_t style_Test_tabview_1_extra_btnm_items_default;
    ui_init_style(&style_Test_tabview_1_extra_btnm_items_default);

    lv_style_set_text_color(&style_Test_tabview_1_extra_btnm_items_default, lv_color_hex(0x4d4d4d));
    lv_style_set_text_font(&style_Test_tabview_1_extra_btnm_items_default, &lv_font_arial_12);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_items_default, LV_PART_ITEMS | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_CHECKED for &style_Test_tabview_1_extra_btnm_items_checked
    static lv_style_t style_Test_tabview_1_extra_btnm_items_checked;
    ui_init_style(&style_Test_tabview_1_extra_btnm_items_checked);

    lv_style_set_text_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_text_font(&style_Test_tabview_1_extra_btnm_items_checked, &lv_font_arial_12);
    lv_style_set_border_width(&style_Test_tabview_1_extra_btnm_items_checked, 4);
    lv_style_set_border_opa(&style_Test_tabview_1_extra_btnm_items_checked, 255);
    lv_style_set_border_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_border_side(&style_Test_tabview_1_extra_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_radius(&style_Test_tabview_1_extra_btnm_items_checked, 0);
    lv_style_set_bg_opa(&style_Test_tabview_1_extra_btnm_items_checked, 60);
    lv_style_set_bg_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_items_checked, LV_PART_ITEMS | LV_STATE_CHECKED);

    // Write codes tab_1
    ui->Test_tabview_1_tab_1 = lv_tabview_add_tab(ui->Test_tabview_1, "NULL");
    ui->Test_tabview_1_tab_1_label = lv_label_create(ui->Test_tabview_1_tab_1);
    lv_label_set_text(ui->Test_tabview_1_tab_1_label, "");

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Tabview2_Test_Initialization(lv_ui *ui)
{
    // Write codes Test_tabview_1
    ui->Test_tabview_1 = lv_tabview_create(ui->Test, LV_DIR_TOP, CIT_UI.LCD_Height / 5.12);
    lv_obj_set_pos(ui->Test_tabview_1, 0, 0);
    lv_obj_set_size(ui->Test_tabview_1, CIT_UI.LCD_Width, CIT_UI.LCD_Height - (CIT_UI.LCD_Height / 5.12));
    lv_obj_set_scrollbar_mode(ui->Test_tabview_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_tabview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_tabview_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_tabview_1, lv_color_hex(0xeaeff3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Test_tabview_1, lv_color_hex(0x4d4d4d), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Test_tabview_1, &lv_font_arial_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Test_tabview_1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_tabview_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for &style_Test_tabview_1_extra_btnm_main_default
    static lv_style_t style_Test_tabview_1_extra_btnm_main_default;
    ui_init_style(&style_Test_tabview_1_extra_btnm_main_default);

    lv_style_set_bg_opa(&style_Test_tabview_1_extra_btnm_main_default, 255);
    lv_style_set_bg_color(&style_Test_tabview_1_extra_btnm_main_default, lv_color_hex(0xffffff));
    lv_style_set_border_width(&style_Test_tabview_1_extra_btnm_main_default, 0);
    lv_style_set_radius(&style_Test_tabview_1_extra_btnm_main_default, 0);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for &style_Test_tabview_1_extra_btnm_items_default
    static lv_style_t style_Test_tabview_1_extra_btnm_items_default;
    ui_init_style(&style_Test_tabview_1_extra_btnm_items_default);

    lv_style_set_text_color(&style_Test_tabview_1_extra_btnm_items_default, lv_color_hex(0x4d4d4d));
    lv_style_set_text_font(&style_Test_tabview_1_extra_btnm_items_default, &lv_font_arial_12);
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_items_default, LV_PART_ITEMS | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_CHECKED for &style_Test_tabview_1_extra_btnm_items_checked
    static lv_style_t style_Test_tabview_1_extra_btnm_items_checked;
    ui_init_style(&style_Test_tabview_1_extra_btnm_items_checked);

    lv_style_set_text_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_text_font(&style_Test_tabview_1_extra_btnm_items_checked, &lv_font_arial_12);
    lv_style_set_border_width(&style_Test_tabview_1_extra_btnm_items_checked, 4);
    lv_style_set_border_opa(&style_Test_tabview_1_extra_btnm_items_checked, 255);
    lv_style_set_border_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_border_side(&style_Test_tabview_1_extra_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_radius(&style_Test_tabview_1_extra_btnm_items_checked, 0);
    lv_style_set_bg_opa(&style_Test_tabview_1_extra_btnm_items_checked, 60);
    lv_style_set_bg_color(&style_Test_tabview_1_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_obj_add_style(lv_tabview_get_tab_btns(ui->Test_tabview_1), &style_Test_tabview_1_extra_btnm_items_checked, LV_PART_ITEMS | LV_STATE_CHECKED);

    // Write codes tab_1
    ui->Test_tabview_1_tab_1 = lv_tabview_add_tab(ui->Test_tabview_1, "NULL");
    ui->Test_tabview_1_tab_1_label = lv_label_create(ui->Test_tabview_1_tab_1);
    lv_label_set_text(ui->Test_tabview_1_tab_1_label, "");

    // Write codes tab_2
    ui->Test_tabview_1_tab_2 = lv_tabview_add_tab(ui->Test_tabview_1, "NULL");
    ui->Test_tabview_1_tab_2_label = lv_label_create(ui->Test_tabview_1_tab_2);
    lv_label_set_text(ui->Test_tabview_1_tab_2_label, "");

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Window_Touch_Test_Initialization(lv_ui *ui)
{
    lv_tabview_rename_tab(ui->Test_tabview_1, 0, "Touch Info");

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Window_Power_Test_Initialization(lv_ui *ui)
{
    analogReadResolution(12); // 开WIFI的时候会重置模拟接口所以每次初始化都要重新设置精度

    // Write codes Test_label_1
    ui->Test_label_1 = lv_label_create(ui->Test_tabview_1_tab_2);
    lv_label_set_text(ui->Test_label_1, "BAT MEAS");
    lv_label_set_long_mode(ui->Test_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->Test_label_1, -5, -5);
    lv_obj_set_size(ui->Test_label_1, 70, 15);
    lv_obj_set_scrollbar_mode(ui->Test_label_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Test_label_1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Test_label_1, &lv_font_arial_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Test_label_1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Test_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes Test_sw_1
    ui->Test_sw_1 = lv_switch_create(ui->Test_tabview_1_tab_2);
    lv_obj_set_pos(ui->Test_sw_1, 70, -7);
    lv_obj_set_size(ui->Test_sw_1, 30, 18);
    lv_obj_set_scrollbar_mode(ui->Test_sw_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for Test_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_sw_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_sw_1, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Test_sw_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_sw_1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Test_sw_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for Test_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->Test_sw_1, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->Test_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->Test_sw_1, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for Test_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Test_sw_1, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Test_sw_1, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Test_sw_1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Test_sw_1, 100, LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_add_state(ui->Test_sw_1, LV_STATE_CHECKED); // 默认打开开关

    lv_tabview_rename_tab(ui->Test_tabview_1, 0, "Power Info");
    lv_tabview_rename_tab(ui->Test_tabview_1, 1, "Power Ctrl");

    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Window_Breathing_Light_Test_Initialization(lv_ui *ui)
{
    lv_label_set_text(ui->Test_label_2, "Breathing light");
    // Update current screen layout.
    lv_obj_update_layout(ui->Test);
}

void Window_WIFI_STA_Test_Initialization(lv_ui *ui)
{
    String temp;

    temp = "Scanning wifi";

    lv_tabview_rename_tab(ui->Test_tabview_1, 0, "WIFI STA Info");
    lv_label_set_text(guider_ui.Test_tabview_1_tab_1_label, temp.c_str());

    // Update current screen layout.
    lv_obj_update_layout(guider_ui.Test);
}

void Window_LCD_Backlight_Test_Loop(void)
{
    if (CIT_UI.Window_Button_Start_Testing_Flag == true)
    {
        CIT_UI.Window_Button_Start_Testing_Flag = false;

        for (uint8_t j = 0; j < 3; j++)
        {
            for (int i = 0; i <= 255; i++)
            {
                ledcWrite(LCD_BL, i);
                delay(2);
            }
            delay(3000);
            for (int i = 255; i > 0; i--)
            {
                ledcWrite(LCD_BL, i);
                delay(5);
            }
            delay(2000);
        }
    }
}

void Window_LCD_Display_Color_Test_Loop(void)
{
    if (CIT_UI.Window_Button_Start_Testing_Flag == true)
    {
        CIT_UI.Window_Button_Start_Testing_Flag = false;

        gfx->fillScreen(RED);
        delay(3000);
        gfx->fillScreen(GREEN);
        delay(3000);
        gfx->fillScreen(BLUE);
        delay(3000);
        gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, 128, 128);
        delay(3000);
        gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, 128, 128);
        delay(3000);
        gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_3, 128, 128);
        delay(3000);
        gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_4, 128, 128);
        delay(3000);

        // Write the load screen code.
        lv_obj_t *act_scr = lv_scr_act();
        lv_disp_t *d = lv_obj_get_disp(act_scr);
        if (d->prev_scr == NULL && (d->scr_to_load == NULL || d->scr_to_load == act_scr))
        {
            if (guider_ui.Test_del == true)
            {
                setup_scr_Test(&guider_ui);
            }
            lv_scr_load_anim(guider_ui.Test, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 200, true);
            guider_ui.Test_del = true;
        }

        CIT_UI.Window_Current_State = CIT_UI.Window_Current_State::Window_LCD_Display_Color_Test;
        CIT_UI.Window_Initialization_Flag = false;
    }
}

void Window_Touch_Test_Loop(void)
{
    String temp;

    temp = "ID: ";
    temp += (String)CST816T->IIC_Read_Device_ID();
    temp += "\nDevice: ";
    if (CST816T->IIC_Read_Device_ID() == 0xB5)
    {
        temp += "CST816T";
        temp += "\n";
        temp += "\nFingers Number: ";
        temp += CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
        temp += "\nGesture: ";

        if (CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID) == "Swipe Up")
        {
            temp += "Swipe Down";
        }
        else if (CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID) == "Swipe Down")
        {
            temp += "Swipe Up";
        }
        else
        {
            temp += CST816T->IIC_Read_Device_State(CST816T->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID);
        }

        temp += "\n";
        temp += "\nTouch X: ";
        temp += CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        temp += "Y: ";
        temp += CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    }
    else
    {
        temp += "NULL";
    }
    temp += "\n\n";

    lv_label_set_text(guider_ui.Test_tabview_1_tab_1_label, temp.c_str());

    // Update current screen layout.
    lv_obj_update_layout(guider_ui.Test);
}

void Window_Power_Test_Loop(void)
{
    String temp;
    String temp2;

    int32_t Device_ID = ETA4662->IIC_Read_Device_ID();
    String Battery_Status = ETA4662->IIC_Read_Device_State(ETA4662->Arduino_IIC_Power::Status_Information::POWER_BATTERY_FAULT_STATUS);
    uint32_t Battery_Voltage = 0;

    if (Battery_Status != "Normal") // 如果电池处于故障状态（这种状态只有在充电时候未接电池或者超过设置的目标充电电压过多才会导致）
    {
        ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                        ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF); // 充电
        Battery_Voltage = 0;
        delay(1000);
    }
    else
    {
        if (lv_obj_has_state(guider_ui.Test_sw_1, LV_STATE_CHECKED) == true) // 判断控制模式的电池测量开关有没有开启
        {
            digitalWrite(Battery_Measurement_Control, LOW); // 开启电池电压测量
            Battery_Voltage = (analogReadMilliVolts(Battery_ADC_Data) * 2);
            digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量
        }
        else
        {
            digitalWrite(Battery_Measurement_Control, HIGH); // 关闭电池电压测量
            Battery_Voltage = (analogReadMilliVolts(Battery_ADC_Data) * 2);
        }
    }

    if (Battery_Voltage > 3000) // 电池电压大于3V就充电否则一律不充电
    {
        ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                        ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON); // 充电
    }
    else
    {
        ETA4662->IIC_Write_Device_State(ETA4662->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                        ETA4662->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF); // 充电
    }

    temp = "ID: ";
    temp += (String)Device_ID;
    temp += "\nDevice: ";
    if (Device_ID == 0x00)
    {
        temp += "ETA4662";
        temp += "\n";
        temp += "\nBattery Fault Status: ";
        temp += Battery_Status;
        temp += "\nCharging Status: ";
        temp += ETA4662->IIC_Read_Device_State(ETA4662->Arduino_IIC_Power::Status_Information::POWER_CHARGING_STATUS);

        temp += "\n";
        temp += "\nBattery Voltage: ";
        temp += (String)Battery_Voltage;
        temp += " mV";

        temp += "\n";
        temp += "\nInput Minimum Voltage Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_MINIMUM_INPUT_VOLTAGE_LIMIT);
        temp += " mV";
        temp += "\nCharging Target Voltage Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_CHARGING_TARGET_VOLTAGE_LIMIT);
        temp += " mV";
        temp += "\nSystem Voltage Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_SYSTEM_VOLTAGE_LIMIT);
        temp += " mV";

        temp += "\n";
        temp += "\nInput Current Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_INPUT_CURRENT_LIMIT);
        temp += " mA";
        temp += "\nFast Charge Current Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_FAST_CHARGING_CURRENT_LIMIT);
        temp += " mA";
        temp += "\nTermination And Precondition Charge Current Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_TERMINATION_PRECHARGE_CHARGING_CURRENT_LIMIT);
        temp += " mA";
        temp += "\nBAT To SYS Discharge Current Limit: ";
        temp += ETA4662->IIC_Read_Device_Value(ETA4662->Arduino_IIC_Power::Value_Information::POWER_BAT_TO_SYS_DISCHARGE_CURRENT_LIMIT);
        temp += " mA";
    }
    else
    {
        temp += "NULL";
    }
    temp += "\n\n";

    lv_label_set_text(guider_ui.Test_tabview_1_tab_1_label, temp.c_str());

    // Update current screen layout.
    lv_obj_update_layout(guider_ui.Test);
}

void Window_Breathing_Light_Test_Loop(void)
{
    if (CIT_UI.Breathing_Light_Brightness < 0)
    {
        CIT_UI.Breathing_Light_State = true;
    }
    else if (CIT_UI.Breathing_Light_Brightness > 255)
    {
        CIT_UI.Breathing_Light_State = false;
    }

    if (CIT_UI.Breathing_Light_State == true)
    {
        CIT_UI.Breathing_Light_Brightness++;
    }
    else
    {
        CIT_UI.Breathing_Light_Brightness--;
    }

    ledcWrite(Breathing_Light, 255 - CIT_UI.Breathing_Light_Brightness);

    lv_slider_set_value(guider_ui.Test_slider_1, CIT_UI.Breathing_Light_Brightness, lv_anim_enable_t::LV_ANIM_OFF);
    // Update current screen layout.
    lv_obj_update_layout(guider_ui.Test);
}

void Window_WIFI_STA_Test_Loop(void)
{
    if (CIT_UI.Window_WIFI_STA_Test_State == true)
    {
        String temp;
        bool wifi_connection_state;
        int wifi_num = 0;
        uint64_t last_tick;

        temp = "Scanning wifi";
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        wifi_num = WiFi.scanNetworks();
        if (wifi_num == 0)
        {
            temp += "\nWiFi scan complete !\nNo wifi discovered.\n";
        }
        else
        {
            temp += "\nWiFi scan complete !\n";
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

        temp += "Connecting to ";
        temp += WIFI_SSID;
        temp += "\n";

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        last_tick = millis();

        while (WiFi.status() != WL_CONNECTED)
        {
            temp += ".";

            if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
            {
                wifi_connection_state = false;
                break;
            }
            else
            {
                wifi_connection_state = true;
            }
            delay(500); // 这个延时必须要有
        }

        if (wifi_connection_state == true)
        {
            temp += "\nThe connection was successful ! \nTakes: ";
            temp += (String)(millis() - last_tick);
            temp += " ms";
        }
        else
        {
            temp += "\nThe connection was fail !";
        }

        if (wifi_connection_state == true)
        {
            // Obtain and set the time from the network time server
            // After successful acquisition, the chip will use the RTC clock to update the holding time
            configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

            delay(3000);

            struct tm timeinfo;
            if (!getLocalTime(&timeinfo, 10000))
            {
                temp += "\n\nFailed to obtain time!";
            }
            else
            {
                temp += "\n\nGet time success";
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

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        delay(100);

        lv_label_set_text(guider_ui.Test_tabview_1_tab_1_label, temp.c_str());

        // Update current screen layout.
        lv_obj_update_layout(guider_ui.Test);

        CIT_UI.Window_WIFI_STA_Test_State = false;
    }
}

/**
 * Create a demo application
 */
void custom_init(lv_ui *ui)
{
}
