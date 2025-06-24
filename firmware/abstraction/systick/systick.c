/******************************************************************************
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup systick
 * @{
 */

/******************************************************************************
 * @file        systick.c
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 *****************************************************************************/
#include "systick.h"

#define SYSTICK_TABLE_ENTRY(ms) ms

typedef struct {
  Systick_GetMs_t ms;
} systick_t;

BL_STATIC BL_CONST systick_t systick = { SYSTICK_CFG(SYSTICK_TABLE_ENTRY) };

BL_UINT32_T Systick_GetMs(void)
{
  /* Get the ms from the configured systick peripheral */
  return systick.ms();
}

/**@} systick */
