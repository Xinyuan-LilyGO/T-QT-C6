/*
 * @Description: ChipScan.ino
 * @Author: LILYGO_L
 * @Date: 2023-09-12 17:09:04
 * @LastEditTime: 2024-07-11 14:20:13
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_DriveBus_Library.h"
#include "cpp_bus_driver_library.h"
#include "pin_config.h"

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
                                                         DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::shared_ptr<cpp_bus_driver::HardwareI2c1> CPP_IIC_Master_Bus =
    std::make_shared<cpp_bus_driver::HardwareI2c1>(IIC_SDA, IIC_SCL);
std::shared_ptr<cpp_bus_driver::HardwareI2c1> SGM41562_IIC_Bus =
    std::make_shared<cpp_bus_driver::HardwareI2c1>(CPP_IIC_Master_Bus);
std::unique_ptr<cpp_bus_driver::Sgm41562xx> SGM41562(
    new cpp_bus_driver::Sgm41562xx(SGM41562_IIC_Bus));

void Chip_Scan(void)
{
    uint64_t chipid;

    chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).

    Serial.printf("\n------------------------------------------------\n");

    Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); // print High 2 bytes
    Serial.printf("%08X\n", (uint32_t)chipid);                       // print Low 4bytes.
    Serial.printf("Chip Revision = %u\n", ESP.getChipRevision());
    Serial.printf("CpuFreqMHz = %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Cycle Count = %u\n", ESP.getCycleCount());
    Serial.printf("SdkVersion = %s\n", ESP.getSdkVersion());

    Serial.printf("\n");

    Serial.printf("Total heap size = %u B\n", ESP.getHeapSize());
    Serial.printf("Available heap = %u B\n", ESP.getFreeHeap());
    Serial.printf("Lowest level of free heap since boot = %u B\n", ESP.getMinFreeHeap());
    Serial.printf("Largest block of heap that can be allocated at once = %u B\n", ESP.getMaxAllocHeap());

    Serial.printf("\n");

    Serial.printf("Total Psram size = %u B\n", ESP.getPsramSize());
    Serial.printf("Available Psram = %u B\n", ESP.getFreePsram());
    Serial.printf("Lowest level of free Psram since boot = %u B\n", ESP.getMinFreePsram());
    Serial.printf("Largest block of Psram that can be allocated at once = %u B\n", ESP.getMaxAllocPsram());

    Serial.printf("\n");

    Serial.printf("Flash chip size: %u B\n", ESP.getFlashChipSize());
    Serial.printf("Flash chip speed: %u Hz\n", ESP.getFlashChipSpeed());

    switch (ESP.getFlashChipMode())
    {
    case FM_QIO:
        Serial.printf("Flash chip mode: FM_QIO \n");
        break;
    case FM_QOUT:
        Serial.printf("Flash chip mode: FM_QOUT \n");
        break;
    case FM_DIO:
        Serial.printf("Flash chip mode: FM_DIO \n");
        break;
    case FM_DOUT:
        Serial.printf("Flash chip mode: FM_DOUT \n");
        break;
    case FM_FAST_READ:
        Serial.printf("Flash chip mode: FM_FAST_READ \n");
        break;
    case FM_SLOW_READ:
        Serial.printf("Flash chip mode: FM_SLOW_READ \n");
        break;
    case FM_UNKNOWN:
        Serial.printf("Flash chip mode: FM_UNKNOWN \n");
        break;

    default:
        break;
    }

    Serial.printf("------------------------------------------------\n");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    i2c_master_bus_handle_t sgm41562_bus_handle = nullptr;
    if (ETA4662->begin() == true)
    {
        Serial.println("ETA4662 initialization successfully");
    }
    else if (i2c_master_get_bus_handle(
                 I2C_NUM_0, &sgm41562_bus_handle) == ESP_OK &&
             CPP_IIC_Master_Bus->set_bus_handle(sgm41562_bus_handle) &&
             SGM41562->Init())
    {
        Serial.println("SGM41562 initialization successfully");
    }
    else
    {
        Serial.println("Power chip initialization failed");
    }
}

void loop()
{
    Chip_Scan();
    delay(1000);
}
