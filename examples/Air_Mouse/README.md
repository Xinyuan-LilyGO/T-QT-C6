# T-QT-C6 Air Mouse(Demo)
## *AI generated

Modular Arduino IDE firmware for a LILYGO T-QT-C6 with the optional
LSM6DSL battery backboard. 

## Requirements

- T-QT-C6 with the LSM6DSL battery backboard
- Arduino IDE 2.x
- ESP32 board package 3.3.9
- Windows, MacOS with Bluetooth LE support
- LVGL 8.3.5
- NimBLE-Arduino 2.3.7
- Arduino_GFX from commit `00dcd6842c4fe10026468dee83511a4e8bfff50a`


## Arduino IDE Setup

1. Open `tqt-c6-airmouse.ino` in Arduino IDE.
2. **In **File > Preferences > Additional boards manager URLs**, add:
   `https://jihulab.com/esp-mirror/espressif/arduino-esp32/-/raw/gh-pages/package_esp32_index_cn.json`(for somebody in China)
3. In Boards Manager, install `esp32 by Espressif Systems` version 3.3.9.
4. In Library Manager, install `lvgl` 8.3.5 and `NimBLE-Arduino` 2.3.7.
5. Download Arduino_GFX at commit
   `00dcd6842c4fe10026468dee83511a4e8bfff50a`, then use
   **Sketch > Include Library > Add .ZIP Library**.
   Remove older Arduino_GFX folders first; multiple installed versions share
   the same library name and Arduino IDE can select an incompatible old copy.
6. Copy this sketch's `lv_conf.h` next to the installed `lvgl` library folder,
   not inside it. 
7. Select **ESP32C6 Dev Module** and set:
   - USB CDC On Boot: Enabled
   - Flash Size: 4 MB
   - Partition Scheme: Huge APP
   - CPU Frequency: 160 MHz
8. Select the board's serial port, then click Verify or Upload.

After flashing, open your computer's Bluetooth settings and pair with
`TQT-C6 Air Mouse`. The screen changes from `PAIRING...` to the control view
only after the BLE link is encrypted. On every connection the firmware
explicitly starts the security procedure, which is important for Windows
bonded reconnects after a board reset.

## Hardware Variants

Defaults are GC9107 LCD with 180-degree rotation and Battery V1.2. Change the two macros at the top of
`BoardConfig.h` if required.

- `TQT_C6_LCD_DRIVER=1`: GC9107
- `TQT_C6_LCD_DRIVER=2`: ST7735
- `TQT_C6_BATTERY_V12=1`: GPIO3 enables the IMU
- `TQT_C6_BATTERY_V12=0`: GPIO3 selects LSM6DSL address `0x6A`


## Tuning

Cursor response parameters are in `CursorEngine.h`. The default axis map is
`gyro Z -> cursor X` and `gyro X -> cursor Y`; change the mapping or signs in
`CursorEngine.cpp` if the physical board orientation differs.

The IMU runs at 104 Hz with a +/-250 dps range. After calibration, a gyro-only
FIFO keeps the motion path fed with the newest sample. If FIFO setup fails, the
driver falls back to status-register reads. At boot the display shows
`CALIBRATING / KEEP STILL`; calibration accepts 160 fresh samples only when
every gyro axis stays below a 0.2 dps standard-deviation threshold. It retries
up to three times, so keep the device upright and stationary until `PAIRING...`
appears. Cursor integration uses elapsed time, which keeps sensitivity stable
when the loop timing changes.

BLE movement notifications are coalesced by a dedicated FreeRTOS task and sent
at a 20-30 ms application interval. A small edge queue preserves
press/release order when a notify must be retried; button edges bypass the
movement pacing. The firmware leaves connection-parameter negotiation to the
host instead of forcing a second update during authentication; advertising
offers a moderate 20-40 ms preference. Disconnect
reasons, negotiated connection parameters, authentication state, and
notification failures are printed at 115200 baud for diagnosis.
Touch-edge cursor suppression is 20 ms.

During an active connection, LVGL is serviced every 25 ms while touch polling
and IMU sampling continue independently. The IMU task yields at each sample
period so the NimBLE host remains responsive, and a shared I2C mutex prevents register transactions from
interleaving across the IMU, touch controller, and PMU. Battery ADC and charge
status are sampled by a separate low-priority task and exposed as atomic cache
values, so the control loop never waits for a power measurement.

If a host reports the device as connected but encryption does not complete for
8 seconds, the firmware drops that stale link and resumes advertising. This
prevents the display from remaining on the pairing screen indefinitely.

Startup prints through UART0 (not USB CDC): GPIO16 is TX0 and GPIO17 is RX0,
115200 baud, 8-N-1. Connect a CH340 as CH340-RX -> GPIO16, CH340-TX -> GPIO17,
and common GND. It also prints the display result, I2C pins/frequency, a full
I2C address scan, and touch-controller ACK/chip-ID results.

For the V1.2 battery backboard, GPIO3 is enabled first to power the bottom
module. Since this application already uses Arduino `Wire` for the IMU and
touch controller, PMU access is implemented directly on that same `Wire` bus;
this avoids the ESP-IDF old/new I2C driver conflict in cpp-bus-driver when
compiled by Arduino-ESP32. PMU interrupt is monitored on GPIO4, and the
battery measurement path is enabled on GPIO8 with ADC input on GPIO6. The
SGM41562 address is `0x03`, matching the official driver. Its initialization
sequence, device-ID check, and charging-status bits are implemented locally,
so cpp-bus-driver is not required by this sketch.

Startup now gates all later initialization on the bottom module: I2C, SGM41562,
and LSM6DSL must all be detected before LCD, LVGL, touch, calibration, and BLE
are started. The control screen shows a voltage-derived battery percentage in
the upper-left corner. It is green while the SGM41562 status register reports
pre-charge or charge, and white otherwise.

## Structure

```text
tqt-c6-airmouse.ino      Arduino entry points only
AppController.*         application orchestration and state transitions
BleMouseService.*       BLE HID mouse reports and connection state
MotionController.*      high-priority IMU sampling and cursor pipeline
DisplayDevice.*         LCD driver wrapper
I2cRegisterDevice.*     shared I2C register access
I2cBusLock.*            shared-bus transaction mutex
ImuDevice.*             LSM6DSL driver
TouchDevice.*           CST816T driver
CursorEngine.*          IMU cursor filtering
TouchButtons.*          touch debounce and mouse button state
LvglPort.*              LVGL display port
UiController.*          pairing and control screens
BoardConfig.h           verified T-QT-C6 board configuration
lv_conf.h               LVGL 8 configuration
```
