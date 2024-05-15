/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_LOADER_H
#define __BL_LOADER_H

/**
 * @addtogroup loader
 * @{
 */

/**************************************************************************//**
 * @file        loader.h
 *
 * @brief       Provides an interface to write the application to the main
 *              partition as well as the oldest backup partition
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-05
 *****************************************************************************/
#include "config.h"

BL_Err_t Loader_Load(void);

/**@} loader */

#endif //__BL_LOADER_H
