/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup bootloader
 * @{
 */

/**
 * @file        bootloader.h
 *
 * @brief       Provides the API for Bootloader Configuration
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-02S
 */

/*****************************************************************************/
#ifndef __Bootloader_H
#define __Bootloader_H

#include "platform.h"

bool Bootloader_Get(void);
void Bootloader_Set(void);

#endif //__Bootloader_H

/**@} bootloader */
