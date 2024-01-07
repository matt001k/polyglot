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
 * @date        2022-09-19
 *****************************************************************************/

#include "config.h"

typedef void (*Serial_Init_t)(void);
typedef void (*Serial_Transmit_t)(BL_UINT8_T *data, BL_UINT32_T length);
typedef void (*Serial_Cb_t)(BL_UINT8_T *data, BL_UINT32_T length);
typedef void (*Serial_RegisterCb_t)(Serial_Cb_t cb);
typedef void (*Serial_DeregisterCb_t)(void);

/**************************************************************************//**
 * @brief Initialize The Configured Serial Peripherals
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Serial_Init(void);

/**************************************************************************//**
 * @brief Flushes the Serial Peripherals
 * 
 * @return BL_Err_t 
 *****************************************************************************/
void Serial_Flush(void);

/**************************************************************************//**
 * @brief Transmit Data Through the Locked Serial Peripheral
 *
 * @details The lock for the serial port must be implemented before this is
 *          accessed. Transmit data to the locked serial peripheral.
 * 
 * @param data[in] data to transmit
 * @param length[in] length of data to transmit
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Serial_Transmit(BL_UINT8_T *data, BL_UINT32_T length);

/**************************************************************************//**
 * @brief Receive Data Through the Locked Serial Peripheral
 * 
 * @details The lock for the serial port must be implemented before this is
 *          accessed. Return buffered data from the registered callback. This
 *          also flushes the buffer
 * 
 * @param data[out] data to receive
 * @param length[in] length of data to receive
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Serial_Receive(BL_UINT8_T *data, BL_UINT32_T length);

/**************************************************************************//**
 * @brief Register a Callback to the Serial Peripheral
 * 
 * @details The length of the returned data is passed into an argument of this
 *          callback. This would be used for buffer count of a command.
 * 
 * @param cb callback to register for the serial peripheral 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Serial_RegisterCb(void (*cb)(BL_UINT32_T length));

/**************************************************************************//**
 * @brief Deregister the Serial Peripheral Callback
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Serial_DeregisterCb(void);

/**************************************************************************//**
 * @brief Unlocks the serial port
 * 
 * @return BL_ERR_t 
 *****************************************************************************/
BL_Err_t Serial_Unlock(void);

/**@} serial */

#endif //__BL_SERIAL_H