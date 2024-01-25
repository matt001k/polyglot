/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2c
 * @{
 */

/**
 * @file        i2c.h
 *
 * @brief       Provides the API for I2C
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-09
 */

/*****************************************************************************/
#ifndef __I2C_H
#define __I2C_H

#include "platform.h"

#define I2C_CONTINUE (0U)
#define I2C_STOP (1U)

typedef enum
{
    I2C_CODEC,
    NUM_I2C
} I2C_Type_t;

/**************************************************************************//**
 * @brief Initialize I2C Peripherals
 * 
 * @details This is responsible for initializing all I2C_Type_t peripherals
 * 
 * @returns None
 *****************************************************************************/
void I2C_Init(void);

/**************************************************************************//**
 * @brief Deinitialize I2C Peripherals
 * 
 * @details This is responsible for deinitializing all I2C_Type_t peripherals
 * 
 * @returns None
 *****************************************************************************/
void I2C_Deinit(void);

/**************************************************************************//**
 * @brief Transmit Data Over I2C Bus
 * 
 * @details Transmits requested number of bytes over I2C bus, can opt to send a
 *          stop condition after the transaction. Useful to continue operations
 *          if immedietly doing a read operation after.
 * 
 * @param type[in] I2C peripheral type
 * @param address[in] slave device address attempted to communicate to
 * @param data[in] data to send to the slave device
 * @param size[in] size of data to send to slave device
 * @param stop[in] whether or not a stop condition is generated after
 * 
 * @return None
 *****************************************************************************/
void I2C_Transmit(I2C_Type_t type,
                  uint8_t address,
                  uint8_t *data,
                  uint32_t size,
                  uint8_t stop);

/**************************************************************************//**
 * @brief Receive Data Over I2C Bus
 * 
 * @details Receives requested number of bytes over I2C bus, can opt to send a
 *          stop condition after the transaction
 * 
 * @param type[in] I2C peripheral type
 * @param address[in] slave device address attempted to communicate to
 * @param data[in] data to receive from the slave device
 * @param size[in] size of data to receive to slave device
 * @param stop[in] whether or not a stop condition is generated after
 * 
 * @return None
 *****************************************************************************/
void I2C_Receive(I2C_Type_t type,
                 uint8_t address,
                 uint8_t *data,
                 uint32_t size,
                 uint8_t stop);

/**************************************************************************//**
 * @brief Flush I2C Rx and Tx Buffers
 * 
 * @param type[in] I2C peripheral type
 * 
 * @return None
 *****************************************************************************/
void I2C_Flush(I2C_Type_t type);

#endif //__I2C_H