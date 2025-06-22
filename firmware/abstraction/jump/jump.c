/******************************************************************************
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup jump
 * @{
 */

/******************************************************************************
 * @file        jump.c
 *
 * @brief       Provides an abstraction layer for the bootloader to jump to the
 *              application
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-29
 *****************************************************************************/
#include "jump.h"

#include "nvm.h"

#define JUMP_FUNCTION(jump) jump

typedef void (*Jump_Cb_t)(BL_UINT32_T address);

BL_STATIC BL_CONST Jump_Cb_t jump_cb = JUMP_CFG(JUMP_FUNCTION);

BL_Err_t Jump_ToApp(void)
{
  BL_Err_t    err     = BL_ERR;
  BL_UINT32_T address = 0;

  if(jump_cb && (err = NVM_GetLocation(APPLICATION_NODE, &address)) == BL_OK) {
    err = BL_OK;
    jump_cb(address);
  }

  return err;
}

/**@} jump */
