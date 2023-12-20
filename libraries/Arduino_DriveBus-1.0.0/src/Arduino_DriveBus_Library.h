/*
 * @Description(CN):
 *      Arduino_DriveBus_Library是使用Arduino_DriveBus库时需要引用的
 *  其中包含了所有需要引用的库 使用的时候只需要调用这个头文件就行了
 *
 * @Description(EN):
 *      Arduino_DriveBus_Library is the header file to be referenced when using
 * the Arduino_DriveBus library, which contains all the necessary libraries to be referenced.
 * Just call this header file when using it.
 *
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-11-16 15:49:20
 * @LastEditors: Xk_w
 * @LastEditTime: 2023-12-19 18:01:12
 * @License: GPL 3.0
 */
#pragma once

#include <iostream>
#include <memory>

#include "Arduino_DriveBus.h"

#include "./hardware/Arduino_HWIIC.h"

#include "Arduino_IIC.h"

#include "./power_chip/Arduino_ETA4662.h"
#include "./power_chip/Arduino_SY6970.h"

#include "./touch_chip/Arduino_CST816x.h"
