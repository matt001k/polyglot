/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_CRC32_H
#define __BL_CRC32_H

/**
 * @addtogroup crc32
 * @{
 */

/**************************************************************************//**
 * @file        crc32.h
 *
 * @brief       Provides an interface for calculating 32 bit CRC of data
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-22
 *****************************************************************************/
#include "config.h"

#define CRC32_SIZE (BL_SIZEOF(BL_UINT32_T))

BL_UINT32_T CRC32(BL_UINT32_T crc,
                  BL_CONST void *buf,
                  BL_UINT32_T size);

/**@} crc32 */

#endif //__BL_CRC32_H