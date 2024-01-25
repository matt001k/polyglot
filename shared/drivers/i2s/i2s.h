/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2s
 * @{
 */

/**
 * @file        i2s.h
 *
 * @brief       Provides the API for I2S peripheral
 *
 * @author      Matthew Krause
 *
 * @date        2022-12-02
 */

/*****************************************************************************/
#ifndef __I2S_H
#define __I2S_H

#include "gpio.h"
#include "platform.h"

typedef enum
{
    I2S_CODEC,
    NUM_I2S,
} I2S_Type_t;

typedef void (*I2S_TxCallback_t)(void);
typedef void (*I2S_RxCallback_t)(void);

/**************************************************************************//**
 * @brief Initialize I2S Peripherals
 * 
 * @details This is responsible for initializing all I2S_Type_t peripherals
 * 
 * @return None
 *****************************************************************************/
void I2S_Init(void);

/**************************************************************************//**
 * @brief Deinitialize I2S Periherals
 * 
 * @details This is responsible for deinitializing all I2S_Type_t peripherals
 * 
 * @return None
 *****************************************************************************/
void I2S_Deinit(void);

/**************************************************************************//**
 * @brief Transmit Data Over I2S Peripheral
 * 
 * @details All parameters must be valid for this function to work
 * 
 * @param type[in] I2S peripheral type to tx
 * @param data[in] data to transmit over the I2S type
 * @param size[in] size of data to send and receive over I2S type
 * 
 * @return None
 *****************************************************************************/
void I2S_Transmit(I2S_Type_t type, uint16_t left, uint16_t right);

/**************************************************************************//**
 * @brief Receive Data Over I2S Peripheral
 * 
 * @details All parameters must be valid for this function to work
 * 
 * @param type[in] I2S peripheral type to rx
 * @param data[in/out] data to receive over the I2S type
 * @param size[in] size of data to send and receive over I2S type
 * 
 * @return None
 *****************************************************************************/
void I2S_Receive(I2S_Type_t type, uint16_t *left, uint16_t *right);

/**************************************************************************//**
 * @brief Register a callback for when a transmission is completed
 * 
 * @details This callback occurs when the full transmission is completed
 * 
 * @param type[in] I2S peripheral type
 * @param cb[in] function pointer to callback required, non-null
 * 
 * @return None
 *****************************************************************************/
void I2S_RegisterTxCallback(I2S_Type_t type, I2S_TxCallback_t cb);

/**************************************************************************//**
 * @brief Register a callback for when a reception is completed
 * 
 * @details This callback occurs when the full reception is completed
 * 
 * @param type[in] I2S peripheral type
 * @param cb[in] function pointer to callback required, non-null
 * 
 * @return None
 *****************************************************************************/
void I2S_RegisterRxCallback(I2S_Type_t type, I2S_RxCallback_t cb);

#endif

/**@} i2s */

