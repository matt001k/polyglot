/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup time
 * @{
 */

/**
 * @file        time.h
 *
 * @brief       Provides the API for time interface
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 */

/*****************************************************************************/
#ifndef __TIME_H
#define __TIME_H

#include "platform.h"

void Time_Init(void);
uint32_t Time_GetRuntimeMs(void);
uint32_t Time_GetRuntimeS(void);

#endif // __TIME_H

/**@} time */
