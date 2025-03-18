<!--
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-11 16:13:14
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2025-03-18 09:38:23
 * @License: GPL 3.0
-->
<h1 align = "center">T-QT-C6</h1>

<p align="center" width="100%">
    <img src="image/14.jpg" alt="">
</p>

<p> 
  <!-- <a href="https://code.visualstudio.com/"> <img src="badges/VisualStudioCode_badge.png" height="25px" alt="VisualStudioCode_badge" /> </a>
  <a href="https://platformio.org/"> <img src="badges/PlatformIO_badge.png" height="25px" alt="PlatformIO_badge" /> </a> -->
  <a href="https://www.arduino.cc/"> <img src="badges/Arduino_badge.png" height="25px" alt="Arduino_badge"></a>
</p> 

## **English | [中文](./README_CN.md)**

## Version iteration:
| Version                              | Update date                       |
| :-------------------------------: | :-------------------------------: |
| T-QT-C6_V1.0            | 2023-12-20                         |

| Version       | Update date   |Update description|
| :----------------: | :--------------: |:--------------: |
| T-QT-C6_V1.0       | 2023-12-20      |Original version      |
| T-QT-C6_V1.1     | 2024-03-27         |New support for battery backboards      |
| T-QT-C6_V1.2      | 2024-06-13        |Modify the power management chip to SGM41562|

## PurchaseLink

| Product                     | SOC           |  FLASH  |  PSRAM   | Link                   |
| :------------------------: | :-----------: |:-------: | :---------: | :------------------: |
| T-QT-C6_V1.0-V1.2   | ESP32C6 |   4M   | -| [LILYGO Mall](https://lilygo.cc/products/t-qt-c6?_pos=1&_sid=543d51cdd&_ss=r) |

## Directory
- [Describe](#describe)
- [Preview](#preview)
- [Module](#module)
- [QuickStart](#quickstart)
- [PinOverview](#pinoverview)
- [RelatedTests](#RelatedTests)
- [FAQ](#faq)
- [Project](#project)
- [Information](#information)
- [DependentLibraries](#dependentlibraries)

## Describe

T-QT-C6 is a mini development board based on the ESP32C6 chip, which is only slightly larger than two thumbs. It features a 128x128px TFT full-color touch screen, a separate charging indicator light, and an independent battery. It can communicate with the power path management chip to modify the power bus status and limit values. When the main chip enters the light_sleep mode, the static current at the battery location is only about 500uA. In deep_sleep mode, it can achieve an extremely low power consumption of 100uA.

The newly updated T-QT-C6_V1.1-V1.2 has added battery bottom board power supply and inertial sensors. The added inertial sensors support step counting, attitude detection, and other functions.

## Preview

### Actual Product Image

<p align="center" width="100%">
    <img src="image/14.jpg" alt="">
</p>

---

<p align="center" width="100%">
    <img src="image/15.jpg" alt="">
</p>

---

<p align="center" width="100%">
    <img src="image/16.jpg" alt="">
</p>

## Module

### 1. MCU

* Module: ESP32-C6-MINI-1U
* Chip: ESP32-C6-FH4
* PSRAM: 4M 
* FLASH: -
* Other: For more information, please visit [Espressif ESP32-C6-MINI-1U datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6-mini-1_mini-1u_datasheet_en.pdf)

### 2. Screen

* Screen Model: N085-1212TBWIG06-C08
* Size: 0.85-inch
* Resolution Ratio: 128x128px
* Screen Type: TFT
* Driver Chip: GC9107
* Bus Communication Protocol: Standard SPI

### 3. Touch chip

* Chip: CST816T
* Bus Communication Protocol: IIC
* Other: Supports up sliding, down sliding, left sliding, right sliding, double clicking, clicking, and long pressing gestures for triggering, as well as a combination of multiple interrupt triggering methods. By default, it automatically enters sleep power-saving mode after a few seconds without touch

### 4. Power Management Chip

> #### T-QT-C6_V1.0-V1.1
> * Chip: ETA4662
> * Bus Communication Protocol: IIC
> * Other: A chip with power path management that automatically recognizes battery power and USB power, automatically switches to USB power when there is no battery inserted, and automatically switches to battery power when there is no USB power

> #### T-QT-C6_V1.2
> * Chip: SGM41562
> * Bus Communication Protocol: IIC
> * Other: A chip with power path management that automatically recognizes battery power and USB power, automatically switches to USB power when there is no battery inserted, and automatically switches to battery power when there is no USB power

### 5. Battery backplate inertial sensor

> #### T-QT-C6_V1.1-V1.2
> * Chip:LSM6DSLTR
> * Bus Communication Protocol: IIC
> * Other: 6-axis sensor, supporting step counting and attitude detection.

## QuickStart

### Attention: Currently, ESP32C6 can only be programmed using the Arduino IDE

### Examples Support

| Example | Support IDE And Version| Description | Picture |
| ------  | ------  | ------ | ------ | 
| [Battery_Voltage](./examples/Battery_Voltage) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [Breathing_Light](./examples/Breathing_Light) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [ChipScan](./examples/ChipScan) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [CST816T](./examples/CST816T) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [Deep_Sleep](./examples/Deep_Sleep) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [Light_Sleep](./examples/Light_Sleep) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [ETA4662](./examples/ETA4662) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [GFX](./examples/GFX) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [IMU](./examples/IMU) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [IMU_Level](./examples/IMU_Level) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [SGM41562](./examples/SGM41562) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |
| [Lvgl_CIT_ETA4662](./examples/Lvgl_CIT_ETA4662) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` | Factory original test documents |  |
| [Lvgl_CIT_SGM41562](./examples/Lvgl_CIT_SGM41562) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` | Factory original test documents |  |
| [Light_Sleep_Wakeup](./examples/Light_Sleep_Wakeup) | `[Arduino IDE][arduino-esp32-libs_v3.0.2]` |  |  |

| Firmware | Description | Picture |
| ------  | ------  | ------ |
| [Lvgl_CIT_ETA4662](./firmware/[TQT-C6_V1.0-V1.1][Lvgl_CIT_ETA4662]_firmware_V1.0.0.bin) | Factory original test documents |  |
| [Lvgl_CIT_SGM41562](./firmware/[TQT-C6_V1.2][Lvgl_CIT_SGM41562]_firmware_V1.0.1.bin) | Factory original test documents |  |

### PlatformIO
1. Install[VisualStudioCode](https://code.visualstudio.com/Download),Choose installation based on your system type.

2. Open the "Extension" section of the Visual Studio Code software sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>" to open the extension),Search for the "PlatformIO IDE" extension and download it.

3. During the installation of the extension, you can go to GitHub to download the program. You can download the main branch by clicking on the "<> Code" with green text, or you can download the program versions from the "Releases" section in the sidebar.

4. After the installation of the extension is completed, open the Explorer in the sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>E</kbd>" go open it),Click on "Open Folder," locate the project code you just downloaded (the entire folder), and click "Add." At this point, the project files will be added to your workspace.

5. Open the "platformio.ini" file in the project folder (PlatformIO will automatically open the "platformio.ini" file corresponding to the added folder). Under the "[platformio]" section, uncomment and select the example program you want to burn (it should start with "default_envs = xxx") Then click "<kbd>[√](image/4.png)</kbd>" in the bottom left corner to compile,If the compilation is correct, connect the microcontroller to the computer and click "<kbd>[→](image/5.png)</kbd>" in the bottom left corner to download the program.

### Arduino
1. Install[Arduino](https://www.arduino.cc/en/software),Choose installation based on your system type.

2. Open the "example" directory within the project folder, select the example project folder, and open the file ending with ".ino" to open the Arduino IDE project workspace.

3. Open the "Tools" menu at the top right -> Select "Board" -> "Board Manager." Find or search for "esp32" and download the board files from the author named "Espressif Systems." Then, go back to the "Board" menu and select the development board type under "ESP32 Arduino." The selected development board type should match the one specified in the "platformio.ini" file under the [env] section with the header "board = xxx." If there is no corresponding development board, you may need to manually add the development board from the "board" directory within your project folder.

4. Open menu bar "[File](image/6.png)" -> "[Preferences](image/6.png)" ,Find "[Sketchbook location](image/7.png)"  here,copy and paste all library files and folders from the "libraries" folder in the project directory into the "libraries" folder in this directory.

5. Select the correct settings in the Tools menu, as shown in the table below.

| Setting                               | Value                                 |
| :-------------------------------: | :-------------------------------: |
| Board                                | ESP32C6 Dev Module            |
| Upload Speed                     | 921600                               |
| CPU Frequency                   | 160MHz                              |
| Flash Mode                         | QIO                                   |
| Flash Size                           | 4MB (32Mb)                     |
| Core Debug Level                | None                                 |
| Partition Scheme                | Huge APP (3MB No OTA/1MB SPIFFS)                                 |

6. Select the correct port.

7. Click "<kbd>[√](image/8.png)</kbd>" in the upper right corner to compile,If the compilation is correct, connect the microcontroller to the computer,Click "<kbd>[→](image/9.png)</kbd>" in the upper right corner to download.

### firmware download
1. Open the project file "tools" and locate the ESP32 burning tool. Open it.

2. Select the correct burning chip and burning method, then click "OK." As shown in the picture, follow steps 1->2->3->4->5 to burn the program. If the burning is not successful, press and hold the "BOOT-0" button and then download and burn again.

3. Burn the file in the root directory of the project file "[firmware](./firmware/)" file,There is a description of the firmware file version inside, just choose the appropriate version to download.

<p align="center" width="100%">
    <img src="image/10.png" alt="example">
    <img src="image/11.png" alt="example">
</p>

## PinOverview

| Screen pins      | ESP32C6 pins     |
| :------------------: | :------------------:|
| MOSI                     | IO15                  |
| SCLK                  | IO18                  |
| RST                    | IO20                  |
| BL                      | IO2                  |
| CS                    | IO14                  |
| DC                    | IO19                  |

| Battery related pins | ESP32C6 pins      |
| :------------------: | :------------------:|
| BATTERY_MEASUREMENT_CONTROL  | IO8                  |
| BATTERY_ADC_DATA                  | IO6                    |

| Breathing light pins          | ESP32C6 pins      |
| :------------------: | :------------------:|
| BREATHING_LIGHT                  | IO9                  |

| Touch chip pins          | ESP32C6 pins      |
| :------------------: | :------------------:|
| RST                  | IO23                  |
| INT                  | IO7                    |
| SDA                  | IO21                  |
| SCL                  | IO22                  |

| Sleep wake up pins          | ESP32C6 pins      |
| :------------------: | :------------------:|
| SLEEP_WAKE_UP_INT      | IO7                  |

| Power management chip pins          | ESP32C6 pins      |
| :------------------: | :------------------:|
| SDA      | IO21                  |
| SCL      | IO22                  |

> #### T-QT-C6_V1.1-V1.2
> | Inertial sensor pins   | ESP32C6 pins      |
> | :------------------: | :------------------:|
> | SDA      | IO21                  |
> | SCL      | IO22                  |
> | LSM6DSL_IIC_ADDRESS_MODE      | IO3      |
> | INT1      | IO0                  |
> | INT2      | IO1                  |

> #### T-QT-C6_V1.2
> | Power management chip pins    | ESP32C6 pins      |
> | :------------------: | :------------------:|
> | INT      | IO4                  |

## RelatedTests

### Power Dissipation
| Firmware | Program| Description | Picture |
| ------  | ------  | ------ | ------ | 
| [[TQT-C6_V1.0-V1.2][Light_Sleep]_firmware_V1.0.0.bin](./firmware/[TQT-C6_V1.0-V1.2][Light_Sleep]_firmware_V1.0.0.bin) | `Light_Sleep` |Power dissipation: 553uA <br /> For more information please refer to [Power Consumption Test Log](./relevant_test/PowerConsumptionTestLog_[T-QT-C6_V1.2]_20241122.pdf) | <p align="center" width="10%"> <img src="image/13.png" alt="example" width="100%"> </p> |
| [[TQT-C6_V1.0-V1.2][Deep_Sleep]_firmware_V1.0.0.bin](./firmware/[TQT-C6_V1.0-V1.2][Deep_Sleep]_firmware_V1.0.0.bin) | `Deep_Sleep` | Power dissipation: 170uA <br /> For more information please refer to [Power Consumption Test Log](./relevant_test/PowerConsumptionTestLog_[T-QT-C6_V1.2]_20241122.pdf) |<p align="center" width="10%"> <img src="image/12.png" alt="example" width="100%"> </p> |

## FAQ

* Q. After reading the above tutorials, I still don't know how to build a programming environment. What should I do?
* A. If you still don't understand how to build an environment after reading the above tutorials, you can refer to the [LilyGo-Document](https://github.com/Xinyuan-LilyGO/LilyGo-Document) document instructions to build it.

<br />

* Q. Why does Arduino IDE prompt me to update library files when I open it? Should I update them or not?
* A. Choose not to update library files. Different versions of library files may not be mutually compatible, so it is not recommended to update library files.

<br />

* Q. Why is there no serial data output on the "Uart" interface on my board? Is it defective and unusable?
* A. The default project configuration uses the USB interface as Uart0 serial output for debugging purposes. The "Uart" interface is connected to Uart0, so it won't output any data without configuration.<br />For PlatformIO users, please open the project file "platformio.ini" and modify the option under "build_flags = xxx" from "-D ARDUINO_USB_CDC_ON_BOOT=true" to "-D ARDUINO_USB_CDC_ON_BOOT=false" to enable external "Uart" interface.<br />For Arduino users, open the "Tools" menu and select "USB CDC On Boot: Disabled" to enable the external "Uart" interface.

<br />

* Q. Why is my board continuously failing to download the program?
* A. Please hold down the "BOOT-0" button and try downloading the program again.

## Project
* [T-QT-C6_V1.0](./project/T-QT-C6_V1.0)
* [T-QT-C6_V1.1](./project/T-QT-C6_V1.1)
* [T-QT-C6_V1.2](./project/T-QT-C6_V1.2)

## Information
* [Espressif](https://www.espressif.com/en/support/documents/technical-documents)
* [ETA4662_V1.8](./information/ETA4662_V1.8.pdf)
* [AN-CST816T-v1](./information/AN-CST816T-v1.pdf)
* [WS2812B-2020](./information/WS2812B-2020.pdf)
* [WS2812C-2020](./information/WS2812C-2020.pdf)
* [SGMICRO-SGM41562XGTR](./information/SGMICRO-SGM41562XGTR.pdf)
* [lsm6dsl](./information/lsm6dsl.pdf)
* [lsm6dsl-stmicroelectronics_en](./information/lsm6dsl-stmicroelectronics_en.pdf)

## DependentLibraries
* [Arduino_DriveBus-1.1.10](https://github.com/Xk-w/Arduino_DriveBus)
* [Arduino_GFX-1.3.7](https://github.com/moononournation/Arduino_GFX)
* [lvgl-8.3.5](https://github.com/lvgl/lvgl)