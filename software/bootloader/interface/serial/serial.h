/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_SERIAL_H
#define __BL_SERIAL_H

/**
 * @addtogroup serial
 * @{
 */

/**************************************************************************//**
 * @file        serial.h
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include <iostream>
#include "common.h"

class Serial
{
public:
   typedef void (*Serial_Init_t)(void);
   typedef void (*Serial_Tx_t)(std::uint8_t *data, std::uint32_t length);
   typedef void (*Serial_Rx_t)(std::uint8_t *data, std::uint32_t length);
   typedef struct
   {
       Serial_Init_t init;
       Serial_Tx_t tx;
       Serial_Rx_t rx;
   } Serial_Cfg_t;
   
   Serial(Serial_Cfg_t cfg);
   Serial();
   ~Serial();
   BL_Err_t Init(Serial_Cfg_t cfg);
   BL_Err_t Transmit(std::uint8_t *data, std::uint32_t length);
   BL_Err_t Receive(std::uint8_t *data, std::uint32_t length);
private:
   Serial_Init_t m_Init;
   Serial_Tx_t m_Tx;
   Serial_Rx_t m_Rx;
};

/**@} serial */

#endif // __BL_SERIAL_H