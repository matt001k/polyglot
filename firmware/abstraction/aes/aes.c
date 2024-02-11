/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        aes.c
 *
 * @brief       Provides an abstraction layer for the AES decryption methods
 *              necessary for ingesting images over serial communication and
 *              ensuring safe application loading.
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-11
 *****************************************************************************/
#include "aes.h"

#define AES_CB_EXPAND(init, decrypt) \
    { init, decrypt },

BL_STATIC AES_t aes =
{
    {0},
    NULL,
    NULL,
    AES_CFG(AES_CB_EXPAND)
};

BL_Err_t AES_Init(void)
{
    BL_Err_t err = BL_EIO;

    if (aes.cb.init)
    {
        err = BL_OK;
        aes.cb.init();
    }

    return err;
}

BL_Err_t AES_SetKey(BL_UINT8_T *key)
{
    BL_Err_t err = BL_EIO;

    if (key)
    {
        err = BL_OK;
        aes.key = key;
    }

    return err;
}

BL_Err_t AES_SetIV(BL_UINT8_T *iv)
{
    BL_Err_t err = BL_EIO;

    if (iv)
    {
        err = BL_OK;
        aes.iv = iv;
    }

    return err;
}

BL_Err_t AES_Decrypt(BL_UINT8_T *input, BL_UINT8_T *output, BL_UINT32_T size)
{
    BL_Err_t err = size % AES_128_IV_SIZE == 0 ? BL_EIO : BL_ENODATA;
    if (aes.cb.decrypt && err == BL_EIO)
    {
        err = aes.cb.decrypt(input, output, size, aes.key, aes.iv) == BL_TRUE 
            ? BL_OK : BL_EACCES;
    }

    return err;
}
