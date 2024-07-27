/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        ecc.c
 *
 * @brief       Provides an abstraction layer for verifying firmware signatures
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-24
 *****************************************************************************/
#include "ecc.h"

#define ECC_VERIFY_CB_EXPAND(key, decrypt) \
    .cb = { key, decrypt }

BL_STATIC ECC_t ecc =
{
    NULL,
    ECC_CFG(ECC_VERIFY_CB_EXPAND)
};

BL_Err_t ECC_Init(void)
{
    BL_Err_t err = BL_EIO;

    if (ecc.cb.key && ecc.cb.verify)
    {
        err = BL_OK;
    }
    return err;
}

BL_Err_t ECC_GetKey(void)
{
    BL_Err_t err = BL_EINVAL;
    ecc.key = ecc.cb.key();
    if (ecc.key)
    {
        err = BL_OK;
    }
    return err;
}

BL_Err_t ECC_Decrypt(BL_UINT8_T *hash,
                     BL_UINT8_T *signature)
{
    BL_Err_t err = hash && signature ? BL_EIO : BL_ENODATA;
    if (err == BL_EIO)
    {
        err = ecc.cb.verify(hash,
                            signature,
                            ecc.key) ==
            BL_TRUE ? BL_OK : BL_ERR;
    }
    return err;
}

