/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_SYSTICK_H
#define __BL_SYSTICK_H

/**
 * @addtogroup systick
 * @{
 */

/**************************************************************************//**
 * @file        systick.h
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 *****************************************************************************/

#include "config.h"

typedef void (*Systick_Init_t)(void);
typedef BL_UINT32_T (*Systick_GetMs_t)(void);

/**************************************************************************//**
 * @brief Initialize The Configured Systick Timer
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Systick_Init(void);

/**************************************************************************//**
 * @brief Get the system ticks in milliseconds
 * 
 * @return BL_UINT32_T system ticks in milliseconds
 *****************************************************************************/
BL_UINT32_T Systick_GetMs(void);

/**@} systick */

#endif //__BL_SYSTICK_H