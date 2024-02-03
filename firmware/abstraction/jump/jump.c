/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup jump
 * @{
 */

/**************************************************************************//**
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

#define JUMP_IDX (0U)
#define JUMP_FUNCTION(jump) \
    {jump},

typedef struct
{
    BL_CONST Jump_Cfg_t *cfg;
    BL_UINT8_T count;
} jump_t;

#ifndef TEST
BL_STATIC BL_CONST Jump_Cfg_t jCfg[] =
{
    JUMP_CFG(JUMP_FUNCTION)
    {0},
};
#else
extern Jump_Cfg_t *jCfg;
#endif

BL_STATIC jump_t jump = {0U};

BL_Err_t Jump_Init(void)
{
    BL_Err_t err = BL_OK;

    jump.cfg = jCfg;

    while (jump.cfg[jump.count].cb != 0)
    {
        jump.count++;
    }

    if (jump.count != 1)
    {
        err = BL_EINVAL;
    }

    return err;
}

BL_Err_t Jump_DeInit(void)
{
    jump.count = 0;
    return BL_OK;
}

BL_Err_t Jump_ToApp(void)
{
    BL_Err_t err = BL_ERR;
    BL_UINT32_T address = 0;
    
    if (jump.cfg[JUMP_IDX].cb &&
        (err = NVM_GetLocation(APPLICATION_NODE, &address)) == BL_OK)
    {
        err = BL_OK;
        jump.cfg[JUMP_IDX].cb(address);
    }

    return err;
}


/**@} jump */
