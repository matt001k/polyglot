/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup uart
 * @{
 */

/**
 * @file        uart.h
 *
 * @brief       Provides the API for UART Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-02
 */

/*****************************************************************************/
#ifndef __UART_H
#define __UART_H

#include "platform.h"

typedef enum
{
    UART_DEBUG,
    NUM_UART
} UART_Type_t;

typedef void (*UART_RxCallback_t)(uint8_t *, uint32_t);
typedef void (*UART_TxCallback_t)(void);

void UART_Init(void);
void UART_Deinit(void);
void UART_Transmit(UART_Type_t type, uint8_t *data, uint32_t size);
void UART_Receive(UART_Type_t type, uint8_t *data, uint32_t size);
void UART_RegisterTxCallback(UART_Type_t type, UART_TxCallback_t cb);
void UART_RegisterRxCallback(UART_Type_t type, UART_RxCallback_t cb);
void UART_DeregisterTxCallback(UART_Type_t type);
void UART_DeregisterRxCallback(UART_Type_t type);
void UART_Flush(UART_Type_t type);

#endif // __UART_H

/**@} uart */