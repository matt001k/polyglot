/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_HOLD_H
#define __BL_HOLD_H

/**
 * @addtogroup hold
 * @{
 */

/**************************************************************************//**
 * @file        hold.h
 *
 * @brief       Provides an abstraction layer for the bootloader to hold the
 *              application from launching if it passes the CRC check
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-29
 *****************************************************************************/
#include "config.h"

typedef BL_BOOL_T (*Hold_Cb_t)(void);

/**************************************************************************//**
 * @brief Initializes the Configured Hold Peripheral
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Hold_Init(void);

/**************************************************************************//**
 * @brief Obtain the Configured Hold Condition 
 * 
 * @param ret[out] whether or not the application will stay in bootloader mode
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Hold_Get(BL_BOOL_T *ret);


/**@} hold */

#endif //__BL_HOLD_H