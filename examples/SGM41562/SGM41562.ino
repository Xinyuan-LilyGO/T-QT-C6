/*
 * @Description: SGM41562系列充电芯片完整信息读取示例
 * @Author: LILYGO_L
 * @LastEditTime: 2026-09-03 18:00:00
 * @License: GPL 3.0
 */
#include <Arduino.h>

#include "cpp_bus_driver_library.h"
#include "pin_config.h"

using cpp_bus_driver::HardwareI2c1;
using cpp_bus_driver::Sgm41562xx;

std::shared_ptr<HardwareI2c1> g_i2c_bus =
    std::make_shared<HardwareI2c1>(IIC_SDA, IIC_SCL);
std::shared_ptr<HardwareI2c1> g_sgm41562_bus =
    std::make_shared<HardwareI2c1>(g_i2c_bus);
Sgm41562xx g_sgm41562(g_sgm41562_bus);

/**
 * @brief 输出布尔状态
 * @param name 状态名称
 * @param enabled true：输出Enabled，false：输出Disabled
 */
void PrintBoolean(const char* name, bool enabled) {
  Serial.printf("%-46s: %s\n", name, enabled ? "Enabled" : "Disabled");
}

/**
 * @brief 判断芯片是否采用S/SA扩展寄存器布局
 * @param chip_type 芯片型号
 * @return 采用扩展寄存器布局返回true，否则返回false
 */
bool HasExtendedRegisterMap(Sgm41562xx::ChipType chip_type) {
  return chip_type == Sgm41562xx::ChipType::kSgm41562S ||
         chip_type == Sgm41562xx::ChipType::kSgm41562Sa;
}

/**
 * @brief 将充电状态转换为字符串
 * @param charge_status 充电状态
 * @return 返回充电状态字符串
 */
const char* ChargeStatusToString(Sgm41562xx::ChargeStatus charge_status) {
  switch (charge_status) {
    case Sgm41562xx::ChargeStatus::kNotCharging:
      return "Not charging";
    case Sgm41562xx::ChargeStatus::kPrecharge:
      return "Precharge";
    case Sgm41562xx::ChargeStatus::kCharging:
      return "Charging";
    case Sgm41562xx::ChargeStatus::kChargeComplete:
      return "Charge complete";
    default:
      return "Unknown";
  }
}

/**
 * @brief 输出芯片实时运行状态
 * @param status 芯片实时运行状态
 */
void PrintChipStatus(const Sgm41562xx::ChipStatus& status) {
  Serial.printf("%-46s: %s\n", "Charge status",
      ChargeStatusToString(status.charge_status));
  PrintBoolean("Input power good", status.input_power_good);
  PrintBoolean(
      "Power path management active", status.power_path_management_active);
  PrintBoolean("Thermal regulation active",
      status.thermal_regulation_active);
  PrintBoolean("Watchdog expired", status.watchdog_expired);
}

/**
 * @brief 输出芯片故障状态
 * @param status 芯片故障状态
 */
void PrintFaultStatus(const Sgm41562xx::FaultStatus& status) {
  PrintBoolean("Input power fault", status.input_power_fault);
  PrintBoolean("Thermal shutdown", status.thermal_shutdown);
  PrintBoolean(
      "Battery overvoltage fault", status.battery_overvoltage_fault);
  PrintBoolean("Safety timer expired", status.safety_timer_expired);
  PrintBoolean("NTC hot", status.ntc_hot);
  PrintBoolean("NTC cold", status.ntc_cold);
}

/**
 * @brief 输出芯片全部常用充电与保护配置
 * @param config 芯片充电配置
 * @param extended true：采用S/SA扩展寄存器布局
 */
void PrintChargerConfig(
    const Sgm41562xx::ChargerConfig& config, bool extended) {
  Serial.printf("%-46s: 0x%02X\n", "I2C address", config.i2c_address);
  PrintBoolean("Charge", config.charge_enabled);
  PrintBoolean("Input high impedance", config.high_impedance_enabled);
  Serial.printf("%-46s: %u s\n", "nINT battery reset pull-down",
      config.reset_pull_down_time_s);
  Serial.printf("%-46s: %u s\n", "Battery FET off time",
      config.battery_fet_off_time_s);
  Serial.printf("%-46s: %u mV\n", "Battery UVLO threshold",
      config.battery_undervoltage_threshold_mv);
  Serial.printf("%-46s: %u mV\n", "Minimum input voltage",
      config.minimum_input_voltage_limit_mv);
  Serial.printf("%-46s: %u mA\n", "Input current limit",
      config.input_current_limit_ma);
  PrintBoolean("Input current limit", config.input_current_limit_enabled);
  if (!extended) {
    PrintBoolean("Input current limit additional 200 mA",
        config.input_current_limit_200_ma_offset_enabled);
  }
  Serial.printf("%-46s: %u mA\n", "Fast charge current",
      config.fast_charge_current_ma);
  PrintBoolean(
      "Quarter charge-current scale",
      config.quarter_charge_current_scale_enabled);
  if (extended) {
    Serial.printf("%-46s: %u mA\n", "Precharge current",
        config.precharge_current_ma);
    PrintBoolean("Precharge current x6",
        config.precharge_current_multiplier_six_enabled);
    PrintBoolean("Termination current x6",
        config.termination_current_multiplier_six_enabled);
  }
  Serial.printf("%-46s: %u mA\n", "Termination current",
      config.termination_current_ma);
  Serial.printf("%-46s: %u mA\n", "BAT to SYS discharge current",
      config.discharge_current_limit_ma);
  Serial.printf("%-46s: %u mV\n", "Charge regulation voltage",
      config.charge_voltage_limit_mv);
  Serial.printf("%-46s: %u mV\n", "Precharge to fast-charge threshold",
      config.precharge_to_fast_charge_threshold_mv);
  Serial.printf("%-46s: %u mV\n", "Recharge voltage difference",
      config.recharge_threshold_mv);
  PrintBoolean(
      "Watchdog in discharge", config.watchdog_in_discharge_enabled);
  PrintBoolean("Watchdog", config.watchdog_enabled);
  Serial.printf("%-46s: %u s\n", "Watchdog timeout",
      config.watchdog_timeout_s);
  PrintBoolean("Charge termination", config.charge_termination_enabled);
  PrintBoolean("Charge safety timer", config.safety_timer_enabled);
  Serial.printf("%-46s: %u h\n", "Charge safety timer duration",
      config.safety_timer_hours);
  PrintBoolean(
      "Charge after termination", config.charge_after_termination_enabled);
  PrintBoolean(
      "PPM safety timer extension", config.safety_timer_extended_in_ppm);
  PrintBoolean("NTC", config.ntc_enabled);
  PrintBoolean("Shipping mode", config.shipping_mode_enabled);
  Serial.printf("%-46s: %u s\n", "Shipping mode delay",
      config.shipping_mode_delay_s);
  PrintBoolean("Input power-good interrupt",
      config.input_power_good_interrupt_enabled);
  PrintBoolean(
      "Charge-complete interrupt", config.charge_complete_interrupt_enabled);
  PrintBoolean(
      "Charge-status interrupt", config.charge_status_interrupt_enabled);
  PrintBoolean("NTC interrupt", config.ntc_interrupt_enabled);
  PrintBoolean("Battery-overvoltage interrupt",
      config.battery_overvoltage_interrupt_enabled);
  PrintBoolean("Input voltage loop", config.input_voltage_loop_enabled);
  PrintBoolean("PCB overtemperature protection",
      config.pcb_overtemperature_protection_enabled);
  Serial.printf("%-46s: %u C\n", "Thermal regulation threshold",
      config.thermal_regulation_threshold_c);
  Serial.printf("%-46s: %u mV\n", "System regulation voltage",
      config.system_voltage_regulation_mv);
  Serial.printf("%-46s: %u mV\n", "Input overvoltage threshold",
      config.input_overvoltage_threshold_mv);
  PrintBoolean(
      "Forced power-path switch", config.force_power_path_switch_enabled);
  PrintBoolean("Battery power", config.battery_power_enabled);
  PrintBoolean("Input overvoltage protection",
      config.input_overvoltage_protection_enabled);
  if (extended) {
    Serial.printf("%-46s: %u ms\n", "nINT exit-shipping delay",
        config.exit_shipping_mode_interrupt_delay_ms);
    Serial.printf("%-46s: %u ms\n", "VIN exit-shipping delay",
        config.exit_shipping_mode_input_delay_ms);
    Serial.printf("%-46s: %u ms\n", "Termination deglitch time",
        config.termination_deglitch_time_ms);
    PrintBoolean(
        "nINT in shipping mode", config.shipping_mode_interrupt_enabled);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BATTERY_ADC_DATA, INPUT_PULLDOWN);
  pinMode(BATTERY_MEASUREMENT_CONTROL, OUTPUT);
  digitalWrite(BATTERY_MEASUREMENT_CONTROL, LOW);
  analogReadResolution(12);

  while (!g_sgm41562.Init()) {
    Serial.println("SGM41562 initialization failed");
    delay(2000);
  }

  // 当前硬件使用两线电池，未连接真实NTC温度传感器，因此主动关闭NTC检测。
  while (!g_sgm41562.SetNtcEnable(false)) {
    Serial.println("SGM41562 NTC检测关闭失败");
    delay(2000);
  }

  Serial.printf("SGM41562 initialization succeeded: %s\n",
      Sgm41562xx::ChipTypeToString(g_sgm41562.GetChipType()));
}

void loop() {
  uint8_t chip_id = 0;
  Sgm41562xx::ChipStatus chip_status;
  Sgm41562xx::FaultStatus fault_status;
  Sgm41562xx::ChargerConfig charger_config;

  const bool chip_id_ok = g_sgm41562.GetChipId(chip_id);
  const bool chip_status_ok = g_sgm41562.GetChipStatus(chip_status);
  const bool fault_status_ok = g_sgm41562.GetFaultStatus(fault_status);
  const bool charger_config_ok =
      g_sgm41562.GetChargerConfig(charger_config);

  Serial.println();
  Serial.println("================ SGM41562 ================");
  Serial.printf("%-46s: %lu s\n", "System uptime",
      static_cast<unsigned long>(millis() / 1000));
  Serial.printf("%-46s: %u mV\n", "Battery voltage (board ADC)",
      analogReadMilliVolts(BATTERY_ADC_DATA) * 2);
  Serial.printf("%-46s: %s\n", "Chip type",
      Sgm41562xx::ChipTypeToString(g_sgm41562.GetChipType()));
  if (chip_id_ok) {
    Serial.printf("%-46s: 0x%02X\n", "Chip ID", chip_id);
  }
  if (chip_status_ok) {
    PrintChipStatus(chip_status);
  }
  if (fault_status_ok) {
    PrintFaultStatus(fault_status);
  }
  if (charger_config_ok) {
    PrintChargerConfig(charger_config,
        HasExtendedRegisterMap(g_sgm41562.GetChipType()));
  }
  if (!chip_id_ok || !chip_status_ok || !fault_status_ok ||
      !charger_config_ok) {
    Serial.println("One or more SGM41562 register reads failed");
  }
  Serial.println("============================================");

  delay(2000);
}
