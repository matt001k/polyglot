/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup spi
 * @{
 */

/**
 * @file        spi.h
 *
 * @brief       Provides the API for GPIO
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __SPI_H
#define __SPI_H

#include "gpio.h"
#include "platform.h"

typedef enum
{
    SPI_FLASH,
    NUM_SPI,
} SPI_Type_t;

typedef void (*SPI_TxCallback_t)(void);
typedef void (*SPI_RxCallback_t)(void);

/**************************************************************************//**
 * @brief Initialize SPI Peripherals
 * 
 * @details This is responsible for initializing all SPI_Type_t peripherals
 * 
 * @return None
 *****************************************************************************/
void SPI_Init(void);

/**************************************************************************//**
 * @brief Deinitialize SPI Periherals
 * 
 * @details This is responsible for deinitializing all SPI_Type_t peripherals
 * 
 * @return None
 *****************************************************************************/
void SPI_Deinit(void);

/**************************************************************************//**
 * @brief Transmit and Receive Data Over SPI Peripheral
 * 
 * @details All parameters must be valid for this function to work
 * 
 * @param type[in] SPI peripheral type to tx/rx
 * @param txData[in] data to transmit over the SPI type
 * @param rxData[in/out] data to receive over the SPI type
 * @param size[in] size of data to send and receive over SPI type
 * 
 * @return None
 *****************************************************************************/
void SPI_TransmitReceive(SPI_Type_t type, uint8_t *txData,
                         uint8_t *rxData, uint32_t size);

/**************************************************************************//**
 * @brief Assert the Required Chip Select Pin
 * 
 * @details This must be called prior to SPI_TransmitReceive for a successful
 *          transmission
 * 
 * @param pin[in] pin to assert
 * 
 * @return None
 *****************************************************************************/
void SPI_AssertCs(GPIO_Pin_t pin);

/**************************************************************************//**
 * @brief Deassert the Required Chip Select Pin
 * 
 * @details This must be called after to SPI_TransmitReceive to end a
 *          transmission
 * 
 * @param pin[in] pin to deassert
 * 
 * @return None
 *****************************************************************************/
void SPI_DeassertCs(GPIO_Pin_t pin);

/**************************************************************************//**
 * @brief Register a callback for when a transmission is completed
 * 
 * @details This callback occurs when the full transmission is completed
 * 
 * @param type[in] SPI peripheral type
 * @param cb[in] function pointer to callback required, non-null
 * 
 * @return None
 *****************************************************************************/
void SPI_RegisterTxCallback(SPI_Type_t type, SPI_TxCallback_t cb);

/**************************************************************************//**
 * @brief Register a callback for when a reception is completed
 * 
 * @details This callback occurs when the full reception is completed
 * 
 * @param type[in] SPI peripheral type
 * @param cb[in] function pointer to callback required, non-null
 * 
 * @return None
 *****************************************************************************/
void SPI_RegisterRxCallback(SPI_Type_t type, SPI_RxCallback_t cb);


#endif

/**@} spi */
