/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_RUN_H
#define __BL_RUN_H

/**
 * @addtogroup run
 * @{
 */

/**************************************************************************//**
 * @file        run.h
 *
 * @brief       Task scheduler to run the required task queued
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "config.h"

/**************************************************************************//**
 * @brief Handles the required task to be ran
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Run(void);


/**@} run */

#endif //__BL_RUN_H