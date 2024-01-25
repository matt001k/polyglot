/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup delay
 * @{
 */

/**
 * @file        delay.h
 *
 * @brief       Provides the API for Delay Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-04
 */

/*****************************************************************************/
#ifndef __DELAY_H
#define __DELAY_H

#include "platform.h"

void Delay_Init(void);
void Delay_Deinit(void);
void Delay_Ms(uint32_t ms);
void Delay_Us(uint32_t us);

#endif

/**@} delay */