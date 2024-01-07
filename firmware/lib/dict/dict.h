/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup dict
 * @{
 */

/**
 * @file        dict.h
 *
 * @brief       Dictionary required for bootloader commands
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#ifndef __DICT_H
#define __DICT_H

#include "config.h"

#define SECRET_KEY_WORD (0x1BADCAFE)
#define SECRET_KEY_SIZE (BL_SIZEOF(BL_UINT32_T))

typedef BL_UINT32_T Dict_Item_t;

enum
{
    BL_WRITE = 0x674F4761,
    BL_VALIDATE = 0x7A306536,
    BL_RUN = 0x75326972,
    BL_ERASE = 0X4B7A4B44,
    BL_LOCK = 0x34594337,
    BL_UNLOCK = 0x5A707169,
    BL_RELEASE_PORT = 0x7A566261,
    BL_READY = 0x6F516C4E,
    BL_ERROR = 0x46756334,
    BL_RESET = 0x5451484B,
};

#endif // __DICT_H

/**@} dict */