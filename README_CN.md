<h1 align="center">T-QT-C6</h1>

## **[English](./README.md)** | 中文

[![License](https://img.shields.io/github/license/Xinyuan-LilyGO/T-QT-C6?style=flat-square)](./LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?style=flat-square&logo=arduino&logoColor=white)](https://www.arduino.cc/en/software)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-supported-F5822A?style=flat-square&logo=platformio&logoColor=white)](https://platformio.org/)

<p align="center">
  <img src="image/14.jpg" alt="T-QT-C6" width="70%">
</p>

## 概述

T-QT-C6 是一款基于 ESP32-C6 的迷你开发板，集成 0.85 英寸 128 × 128 LCD、电容触摸、电池电压检测、充电管理和 RGB 呼吸灯。可选电池背板提供电池接口和 LSM6DSL 惯性传感器。

[前往 LILYGO 商城购买](https://lilygo.cc/products/t-qt-c6)

## 目录

- [硬件版本](#硬件版本)
- [使用说明](#使用说明)
- [产品预览](#产品预览)
- [支持的开发环境](#支持的开发环境)
- [快速开始](#快速开始)
- [示例程序](#示例程序)
- [预编译固件](#预编译固件)
- [硬件模块](#硬件模块)
- [引脚总览](#引脚总览)
- [功耗数据](#功耗数据)
- [项目资料](#项目资料)
- [常见问题](#常见问题)

## 硬件版本

| 版本 | 发布日期 | 主要变更 |
| :-- | :-- | :-- |
| T-QT-C6 V1.0 | 2023-12-20 | 初始版本 |
| T-QT-C6 V1.1 | 2024-03-27 | 新增电池背板支持 |
| T-QT-C6 V1.2 | 2024-06-13 | 充电芯片更换为 SGM41562 |
| T-QT-C6 Battery V1.2 | 2025-08-11 | 电池接口更换为 2Pin、1.25 mm 间距连接器 |

## 使用说明

- 请在 [`libraries/common/pin_config.h`](./libraries/common/pin_config.h) 中选择实际连接的电池背板版本，默认配置为 `T_QT_C6_Battery_V1_2`。
- Battery V1.2 使用双线电池接口，仅连接 VBAT 和 GND，没有连接外部 NTC 热敏电阻，因此 SGM41562 应用示例会主动关闭外部 NTC 检测。
- `Arduino_GFX` 和 `cpp_bus_driver` 使用 Git 子模块管理。编译前请递归克隆仓库或手动初始化子模块。
- 工厂测试示例必须与 MCU 板载充电芯片匹配：MCU V1.0/V1.1 使用 `Lvgl_CIT_ETA4662`，MCU V1.2 使用 `Lvgl_CIT_SGM41562`。

## 产品预览

<p align="center">
  <img src="image/14.jpg" alt="T-QT-C6 正面" width="30%">
  <img src="image/15.jpg" alt="T-QT-C6 侧面" width="30%">
  <img src="image/16.jpg" alt="T-QT-C6 背面" width="30%">
</p>

## 支持的开发环境

| 开发环境 | 状态 | 配置来源 |
| :-- | :--: | :-- |
| Arduino IDE | 支持 | 使用 [`platformio.ini`](./platformio.ini) 中注明的 Arduino-ESP32 版本 |
| PlatformIO | 推荐 | 使用仓库提供的 [`platformio.ini`](./platformio.ini) |

框架版本、开发板目标、Flash 分区、USB CDC 和编译环境统一在 [`platformio.ini`](./platformio.ini) 中维护，作为项目配置的唯一来源。

## 快速开始

### 获取源码

克隆项目及其子模块：

```bash
git clone --recursive https://github.com/Xinyuan-LilyGO/T-QT-C6.git
cd T-QT-C6
```

如果已经克隆过项目，请执行以下命令初始化子模块：

```bash
git submodule update --init --recursive
```

### PlatformIO

1. 安装 [Visual Studio Code](https://code.visualstudio.com/) 和 [PlatformIO IDE 扩展](https://platformio.org/install/ide?install=vscode)。

2. 打开 [`platformio.ini`](./platformio.ini)，将 `default_envs` 设置为需要使用的示例，例如：

   ```ini
   [platformio]
   default_envs = GFX
   ```

3. 编译、上传并打开串口监视器：

   ```bash
   pio run
   pio run -t upload
   pio device monitor -b 115200
   ```

示例源码目录由 `default_envs` 动态生成，因此所选环境必须与 `default_envs` 保持一致。

### Arduino IDE

1. 将本仓库 [`libraries/`](./libraries) 目录下的所有库文件夹复制到 Arduino Sketchbook 的 `libraries` 目录。Sketchbook 路径可在 **Arduino IDE > 文件 > 首选项** 中查看；Windows 默认目标路径通常为 `Documents\Arduino\libraries`。

2. 打开所需示例目录中的 `.ino` 文件。

3. 使用以下 Arduino IDE 开发板配置：

| 设置 | 配置值 |
| :-- | :-- |
| Board | ESP32C6 Dev Module |
| Upload Speed | 921600 |
| CPU Frequency | 160MHz |
| Flash Mode | QIO |
| Flash Size | 4MB (32Mb) |
| Core Debug Level | None |
| Partition Scheme | Huge APP (3MB No OTA/1MB SPIFFS) |

### 示例程序

| 示例链接 | 说明 |
| :-- | :-- |
| [`examples/`](./examples) | 当前 T-QT-C6 示例源码 |

请打开示例目录查看最新源码和支持的示例列表。进行工厂测试时，请按照[使用说明](#使用说明)选择与板载充电芯片匹配的环境。

### 预编译固件

以下是已经编译好的固件。

烧录预编译固件时，可参考乐鑫官方 [ESP 固件在线烧录平台说明](https://docs.espressif.com/projects/esp-techpedia/zh_CN/latest/esp-friends/get-started/try-firmware/try-firmware-platform.html)。

| 固件 | 烧录地址 | 说明 |
| --- | --- | --- |
| [`firmware/`](./firmware) | `0x0 (merged)` | T-QT-C6 出厂测试和睡眠示例固件 |

> [!IMPORTANT]
> 烧录 T-QT-C6 固件时选择 `ESP32-C6`。请根据文件名同时匹配 MCU 板版本和电池背板版本，不同硬件版本的固件不能混用。

PlatformIO 编译产物位于 `.pio/build/<环境名称>/`。`firmware.bin` 是应用程序镜像，`firmware.factory.bin` 是包含引导程序、分区表、引导应用和程序固件的合并镜像。`.pio` 是临时生成目录，清理或重新编译时可能被重建，需要长期保留的固件请复制到其他目录。

## 硬件模块

| 模块 | 器件 | 接口或规格 |
| :-- | :-- | :-- |
| MCU 模组 | ESP32-C6-MINI-1U | RISC-V、160 MHz、Wi-Fi 6、低功耗蓝牙、IEEE 802.15.4 |
| Flash | 芯片内置 | 4 MB |
| PSRAM | — | 未配置 |
| LCD | GC9107（早期）或 ST7735（最新） | 0.85 英寸、128 × 128、SPI |
| 触摸 | CST816T | 电容触摸、I2C |
| MCU V1.0/V1.1 充电芯片 | ETA4662 | I2C |
| MCU V1.2 充电芯片 | SGM41562 | I2C |
| 电池背板 IMU | LSM6DSL | 六轴加速度计和陀螺仪、I2C |
| RGB 呼吸灯 | WS2812B/WS2812C | 单总线控制 |

## 引脚总览

开发板的引脚定义和硬件映射位于以下配置文件：

- [`pin_config.h`](./libraries/common/pin_config.h)

## 功耗数据

以下数据使用 T-QT-C6 V1.2 测得，具体测试条件请参阅[功耗测试报告目录](./relevant_test)。

| 模式 | 典型实测电流 |
| :-- | :--: |
| 轻度睡眠 | 553 µA |
| 深度睡眠 | 170 µA |

## 项目资料

- [原理图](./project)
- [数据手册](./information)
- [测试报告](./relevant_test)
- [预编译固件](./firmware)

## 常见问题

<details>
<summary>编译时提示缺少 Arduino_GFX 或 cpp_bus_driver 头文件</summary>

请初始化 Git 子模块后重新编译：

```bash
git submodule update --init --recursive
```

</details>

<details>
<summary>编译了错误的示例</summary>

请在 [`platformio.ini`](./platformio.ini) 中将 `default_envs` 设置为需要的示例。本项目根据该值生成 `src_dir`，因此只修改命令行环境名称并不足够。

</details>

<details>
<summary>无法开始上传或无法识别开发板</summary>

请使用支持数据传输的 USB 线连接开发板。如果无法自动进入下载模式，请按住 **BOOT** 按键并开始上传，在开始写入后松开按键。

</details>

<details>
<summary>串口没有输出</summary>

请使用 `115200` 波特率，并选择 ESP32-C6 的 USB Serial/JTAG 串口。本项目提供的 PlatformIO 配置已启用 USB CDC On Boot。

</details>

<details>
<summary>编译生成的固件在哪里</summary>

固件位于 `.pio/build/<环境名称>/`。如需单个完整工厂镜像，请使用 `firmware.factory.bin`。由于 `.pio` 是自动生成的编译目录，需要保留的镜像请及时复制到其他位置。

</details>
