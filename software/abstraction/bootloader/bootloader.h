/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

/**
 * @addtogroup bootloader
 * @{
 */

/**************************************************************************//**
 * @file        bootloader.h
 *
 * @brief       Abstraction for required API needed for the bootloader library
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-02
 *****************************************************************************/
#include <iostream>
#include "utility.h"
#include "serial.h"
#include "SLABHIDtoUART.h"

class Bootloader
{
public:
    Serial USB;
    static std::uint16_t Vid;
    static std::uint16_t Pid;
    static HID_UART_DEVICE Device;
    Bootloader(void);
    ~Bootloader();
    static void Init(void);
    static void Transmit(std::uint8_t *data, std::uint32_t length);
    static void Receive(std::uint8_t *data, std::uint32_t length);
private:
    static void Exception_Handler(bool_t condition,
                                  std::int32_t line);
    static void Exception_Handler(bool_t condition,
                                  std::int32_t line,
                                  std::string text);
};

/**@} bootloader */

#endif // __BOOTLOADER_H
