/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_UPDATE_H
#define __BL_UPDATE_H

/**
 * @addtogroup update
 * @{
 */

/**************************************************************************//**
 * @file        update.h
 *
 * @brief       Task to update the application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-30
 *****************************************************************************/
#include "config.h"

/**************************************************************************//**
 * @brief Initialize the update task
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Update_Init(void);

/**@} update */

#endif //__BL_UPDATE_H