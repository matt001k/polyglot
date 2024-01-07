/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_BLINK_H
#define __BL_BLINK_H

/**
 * @addtogroup blink
 * @{
 */

/**************************************************************************//**
 * @file        blink.h
 *
 * @brief       Lays out the blink task to blink required LEDs
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "config.h"

/**************************************************************************//**
 * @brief Initialize the Blink Task
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Blink_Init(void);


/**@} blink */

#endif //__BL_BLINK_H