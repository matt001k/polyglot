/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_JUMP_H
#define __BL_JUMP_H

/**
 * @addtogroup jump
 * @{
 */

/**************************************************************************//**
 * @file        jump.h
 *
 * @brief       Provides an abstraction layer for the bootloader to jump to the
 *              application
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-29
 *****************************************************************************/
#include "config.h"

typedef void (*Jump_Cb_t)(BL_UINT32_T address);
typedef struct
{
    Jump_Cb_t cb;
} Jump_Cfg_t;


/**************************************************************************//**
 * @brief initializes the configured jump peripheral
 *
 * @return bl_err_t
 *****************************************************************************/
BL_Err_t Jump_Init(void);

/**************************************************************************//**
 * @brief de-initializes the configured jump peripheral
 *
 * @return bl_err_t
 *****************************************************************************/
BL_Err_t Jump_DeInit(void);

/**************************************************************************//**
 * @brief Jumps to The Application
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Jump_ToApp(void);


/**@} jump */

#endif //__BL_JUMP_H
