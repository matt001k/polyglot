/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup buffer
 * @{
 */

/**************************************************************************//**
 * @file        buffer.c
 *
 * @brief       API to access the working buffer of the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-02
 *****************************************************************************/
#include "buffer.h"

BL_STATIC BL_UINT8_T buf[BL_BUFFER_SIZE] = {0U};

BL_UINT8_T *Buffer_Get(void)
{
    return buf;
}

/**@} buffer */
