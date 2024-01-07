/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_BUFFER_H
#define __BL_BUFFER_H

/**
 * @addtogroup buffer
 * @{
 */

/**************************************************************************//**
 * @file        buffer.h
 *
 * @brief       API to access the working buffer of the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-02
 *****************************************************************************/
#include "config.h"

BL_UINT8_T *Buffer_Get(void);

#endif //__BL_BUFFER_H

/**@} buffer */
