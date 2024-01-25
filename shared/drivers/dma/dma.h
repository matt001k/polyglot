/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup dma
 * @{
 */

/**
 * @file        dma.h
 *
 * @brief       Provides the API for DMA Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-03
 */

/*****************************************************************************/
#ifndef __DMA_H
#define __DMA_H

#include "platform.h"

typedef enum
{
    UART_DEBUG_DMA,
    SPI_FLASH_TX_DMA,
    SPI_FLASH_RX_DMA,
    NUM_DMA,
} DMA_Channel_t;

typedef void (*DMA_Callback_t)(void*);

void DMA_Init(void);
void DMA_Deinit(void);
uint32_t DMA_StartTransfer(DMA_Channel_t channel, void *data, uint32_t size);
void DMA_RegisterCb(DMA_Channel_t channel, DMA_Callback_t cb, void *arg);

#endif // __DMA_H

/**@} dma */
