/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup cpu
 * @{
 */

/**
 * @file        cpu.c
 *
 * @brief       Provides the API for CPU Configuration
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-02S
 */

/*****************************************************************************/
#include "cpu.h"

void CPU_Init(void)
{
    /* Set digital LDO power to utilize DVDD pin */
    EMU->PWRCTRL |= EMU_PWRCTRL_REGPWRSEL_DVDD;
}

void CPU_Deinit(void)
{

}

/**@} cpu */
