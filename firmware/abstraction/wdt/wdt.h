/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_WDT_H
#define __BL_WDT_H

/**
 * @addtogroup wdt
 * @{
 */

/**************************************************************************//**
 * @file        wdt.h
 *
 * @brief       Provides an abstraction layer for the bootloader's watchdog
 *              timer
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "config.h"

typedef void (*WDT_Init_t)(void);
typedef void (*WDT_Kick_t)(void);

/**************************************************************************//**
 * @brief Initialize The Configured Watchdog Timer
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t WDT_Init(void);

/**************************************************************************//**
 * @brief Kick the dog
 *
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t WDT_Kick(void);

/**@} wdt */

#endif //__BL_WDT_H