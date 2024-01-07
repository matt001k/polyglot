/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup bootloader
 * @{
 */

/**************************************************************************//**
 * @file        bootloader.cpp
 *
 * @brief       Abstraction for required API needed for the bootloader library
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-02
 *****************************************************************************/
#include "bootloader.h"
#include "silabs_defs.h"

#define CP2110_TIMEOUT_MS               500000U
#define SILABS_VID                      SILABS_VID_10C4
#define CP2110_PID                      SILABS_PID_CP2110

std::uint16_t Bootloader::Vid = SILABS_VID;
std::uint16_t Bootloader::Pid = CP2110_PID;
HID_UART_DEVICE Bootloader::Device = NULL;


Bootloader::Bootloader(void)
{
    Serial::Serial_Cfg_t cfg =
    {
        Init,
        Transmit,
        Receive,
    };

    USB.Init(cfg);
}

Bootloader::~Bootloader()
{

}

void Bootloader::Init(void)
{
    HID_UART_STATUS status;
    std::uint32_t n = 0U;

    try
    {
        status = HidUart_GetNumDevices(&n, Vid, Pid);
        Exception_Handler(status == HID_UART_DEVICE_NOT_FOUND || n == 0,
                          __LINE__);
        status = HidUart_Open(&Device, 0U, Vid, Pid);
        Exception_Handler(status != HID_UART_SUCCESS, __LINE__);
        status = HidUart_SetTimeouts(Device,
                                     CP2110_TIMEOUT_MS,
                                     CP2110_TIMEOUT_MS);
        status = HidUart_FlushBuffers(Device, true, true);
        Exception_Handler(status != HID_UART_SUCCESS, __LINE__);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}

void Bootloader::Transmit(std::uint8_t *data, std::uint32_t length)
{
    HID_UART_STATUS status;
    std::uint32_t wLength = 0U;

    try
    {
        status = HidUart_Write(Device, data, length, &wLength);
        Exception_Handler(status != HID_UART_SUCCESS || wLength != length,
                          __LINE__);
    }
    catch(const std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Bootloader::Receive(std::uint8_t *data, std::uint32_t length)
{
    HID_UART_STATUS status;
    std::uint32_t rLength = 0U;

    try
    {
        Exception_Handler(length > HID_UART_MAX_READ_SIZE, __LINE__);
        status = HidUart_FlushBuffers(Device, false, true);
        Exception_Handler(status != HID_UART_SUCCESS,
                          __LINE__, "Flush Failed");
        status = HidUart_Read(Device, data, length, &rLength);
        Exception_Handler(status != HID_UART_SUCCESS || rLength != length,
                          __LINE__, "Receiver Timeout");
    }
    catch(const std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Bootloader::Exception_Handler(bool_t condition,
                              std::int32_t line)
{
    if (condition)
    {
        throw std::runtime_error("CP2110 Exception Caught at line: " +
                                 std::to_string(line) + "\r\n");
    }
}

void Bootloader::Exception_Handler(bool_t condition,
                                   std::int32_t line,
                                   std::string text)
{
    if (condition)
    {
        throw std::runtime_error("CP2110 Exception Caught at line: " +
                                 std::to_string(line) + "\r\n" +
                                 "Reason: " + text + "\r\n");
    }
}

/**@} bootloader */

