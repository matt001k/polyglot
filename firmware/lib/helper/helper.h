
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

#define MEMCPY BL_MemCpy
#define MEMSET BL_MemSet
#define UINT32_UINT8 BL_32to8
#define UINT8_UINT32 BL_8to32

#define POLL_DMA_FUNCTION(error, f) \
    while ((error = f) == BL_EALREADY) {};

 void *BL_MemCpy(void *dest,
                 void *src,
                 BL_UINT32_T length);
 void *BL_MemSet(void *dest,
                 BL_UINT8_T data,
                 BL_UINT32_T length);
BL_STATIC BL_INLINE void BL_32to8(BL_UINT8_T *dest,
                                  BL_UINT32_T src);
BL_STATIC BL_INLINE void BL_8to32(BL_UINT32_T *dest,
                                  BL_UINT8_T *src);


BL_STATIC BL_INLINE void BL_32to8(BL_UINT8_T *dest,
                                   BL_UINT32_T src)
{
    BL_INT8_T i = BL_SIZEOF(BL_UINT32_T) - 1U;

    dest[i--] = (BL_UINT8_T) (src);
    for (; i >= 0; --i)
    {
        dest[i] = (BL_UINT8_T) (src >>= 8U);
    }
}

BL_STATIC BL_INLINE void BL_8to32(BL_UINT32_T *dest,
                                  BL_UINT8_T *src)
{
    *dest = (BL_UINT32_T) (src[0] << 24U |
                           src[1] << 16U |
                           src[2] << 8U |
                           src[3]);
}

/**@} helper */

#endif //__BL_HELPER_H
