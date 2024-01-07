/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_LED_H
#define __BL_LED_H

/**
 * @addtogroup led
 * @{
 */

/**************************************************************************//**
 * @file        led.h
 *
 * @brief       Provides an abstraction layer for the bootloader's led
 *              interface
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-19
 *****************************************************************************/
#include "config.h"

#define LED_PERIOD_MIN (100U)

typedef void (*LED_Toggle_t)(void);

typedef BL_UINT8_T BL_LED_T;

/**************************************************************************//**
 * @brief Initialize the Configured LEDs
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t LED_Init(void);

/**************************************************************************//**
 * @brief Obtain the count of all the configured LEDs
 * 
 * @param count[out] number of LEDs configured
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t LED_GetCount(BL_UINT8_T *count);

/**************************************************************************//**
 * @brief Obtain the blinking period of a specific LED
 * 
 * @param num[in] led number
 * @param period[out] period of the requested led
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t LED_GetPeriod(BL_LED_T num, BL_UINT32_T *period);

/**************************************************************************//**
 * @brief Toggle a specified LED
 * 
 * @param num[in] led number
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t LED_Toggle(BL_LED_T num);

/**@} led */

#endif //__BL_LED_H