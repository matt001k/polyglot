/******************************************************************************
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/******************************************************************************
 * @file        helper.c
 *
 * @brief       Provides helper API to the bootloader
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-05
 *****************************************************************************/
#include "helper.h"

void *BL_MemCpy(void *dest,
                void *src,
                BL_UINT32_T length)
{
    BL_UINT8_T *d = (BL_UINT8_T *) dest;
    BL_UINT8_T *s = (BL_UINT8_T *) src;
    while (length--)
    {
        *d++ = *s++;
    }
    return dest;
}

void *BL_MemSet(void *dest,
                BL_UINT8_T data,
                BL_UINT32_T length)
{
    BL_UINT8_T *p = (BL_UINT8_T *) dest;
    while (length-- > 0)
    {
        *p++ = data;
    }
    return dest;
}

void BL_32to8(BL_UINT8_T *dest,
                BL_UINT32_T src)
{
    BL_INT8_T i = BL_SIZEOF(BL_UINT32_T) - 1U;

    dest[i--] = (BL_UINT8_T) (src);
    for (; i >= 0; --i)
    {
        dest[i] = (BL_UINT8_T) (src >>= 8U);
    }
}

void BL_8to32(BL_UINT32_T *dest,
                BL_UINT8_T *src)
{
    *dest = (BL_UINT32_T) (src[0] << 24U |
                           src[1] << 16U |
                           src[2] << 8U |
                           src[3]);
}

