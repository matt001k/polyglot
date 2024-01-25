
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup init
 * @{
 */

/**************************************************************************//**
 * @file        init.c
 *
 * @brief       Provides an abstraction layer for user initialization
 *              customization
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-21
 *****************************************************************************/
#include "init.h"

#define INIT_FUNCTIONS(entry) \
    entry();

void Init_Init(void)
{
    INIT_CFG(INIT_FUNCTIONS)
}

/**@} init */

