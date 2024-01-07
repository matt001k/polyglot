/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_INIT_H
#define __BL_INIT_H

/**
 * @addtogroup init
 * @{
 */

/**************************************************************************//**
 * @file        init.h
 *
 * @brief       Provides an abstraction layer for user initialization
 *              customization
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-21
 *****************************************************************************/
#include "config.h"

/**************************************************************************//**
 * @brief Initialization available to user
 * 
 * @return BL_Err_t 
 *****************************************************************************/
void Init_Init(void);

/**@} init */

#endif //__BL_INIT_H
