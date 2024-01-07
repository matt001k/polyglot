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

BL_Err_t Loader_Init(BL_UINT8_T *buf, BL_UINT32_T size);
BL_Err_t Loader_Write(BL_UINT8_T *data, BL_UINT32_T length);
BL_Err_t Loader_WriteSecret(BL_UINT8_T *data, BL_UINT32_T length);
BL_Err_t Loader_Validate(BL_UINT8_T *data, BL_UINT32_T length);
BL_Err_t Loader_UpdateRevisions(BL_UINT8_T *data, BL_UINT32_T length);
BL_Err_t Loader_SetCustomNodes(BL_UINT8_T *nodes, BL_UINT8_T count);
BL_Err_t Loader_Reset(void);


/**@} loader */

#endif //__BL_LOADER_H