/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        verify.c
 *
 * @brief       Provides an abstraction layer for verifying firmware signatures
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-24
 *****************************************************************************/
#include "verify.h"

#define VERIFY_CB_EXPAND(key, decrypt) \
    .cb = { key, decrypt }

BL_STATIC Verify_t verify =
{
    NULL,
    VERIFY_CFG(VERIFY_CB_EXPAND)
};

BL_Err_t Verify_Init(void)
{
    BL_Err_t err = BL_EIO;

    if (verify.cb.key && verify.cb.verify)
    {
        err = BL_OK;
    }
    return err;
}

BL_Err_t Verify_GetKey(void)
{
    BL_Err_t err = BL_EINVAL;
    verify.key = verify.cb.key();
    if (verify.key)
    {
        err = BL_OK;
    }
    return err;
}

BL_Err_t Verify_Decrypt(BL_UINT8_T *hash,
                        BL_UINT8_T *signature)
{
    BL_Err_t err = hash && signature ? BL_EIO : BL_ENODATA;
    if (err == BL_EIO)
    {
        err = verify.cb.verify(hash,
                               signature,
                               verify.key) ==
            true ? BL_OK : BL_EACCES;
    }
    return err;
}

