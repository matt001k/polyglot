
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_HELPER_H
#define __BL_HELPER_H

/**
 * @addtogroup helper
 * @{
 */

/**************************************************************************//**
 * @file        helper.h
 *
 * @brief       Provides helper API to the bootloader
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "config.h"

#define FLAG_SET (1)
#define FLAG_CLEAR (0)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define MEMCPY BL_MemCpy
#define MEMSET BL_MemSet
#define UINT32_UINT8 BL_32to8
#define UINT8_UINT32 BL_8to32

#define POLL_DMA_FUNCTION(error, f) \
    while ((error = f) == BL_EINPROGRESS) {};

 void *BL_MemCpy(void *dest,
                 void *src,
                 BL_UINT32_T length);
 void *BL_MemSet(void *dest,
                 BL_UINT8_T data,
                 BL_UINT32_T length);
void BL_32to8(BL_UINT8_T *dest,
                BL_UINT32_T src);
void BL_8to32(BL_UINT32_T *dest,
                BL_UINT8_T *src);


/**@} helper */

#endif //__BL_HELPER_H
