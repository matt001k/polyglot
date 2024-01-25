/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup clock
 * @{
 */

/**
 * @file        clock.h
 *
 * @brief       Provides the API for Main Clock Configuration
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-01
 */

/*****************************************************************************/
#ifndef __CLOCK_H
#define __CLOCK_H

#include "platform.h"

void Clock_Init(uint32_t freq);
void Clock_Deinit(void);
uint32_t Clock_GetFrequency(uint32_t clock);
void Clock_OutputInit(void);
void Clock_OutputStart(void);
void Clock_OutputStop(void);

#endif // __CLOCK_H

/**@} clock */