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
#include "helper.h"

#define AES_CB_EXPAND(key, iv, decrypt) \
    .cb = { key, iv, decrypt },

BL_STATIC AES_t aes =
{
    {0},
    {0},
    AES_CFG(AES_CB_EXPAND)
};

BL_Err_t AES_Init(void)
{
    BL_Err_t err = BL_EIO;

    if (aes.cb.key && aes.cb.iv && aes.key)
    {
        err = BL_OK;
    }

    return err;
}

BL_Err_t AES_SetKey(void)
{
    BL_Err_t err = BL_OK;
    MEMCPY(aes.key, aes.cb.key(), AES_KEY_SIZE);
    return err;
}

BL_Err_t AES_SetIV(void)
{
    BL_Err_t err = BL_OK;
    MEMCPY(aes.iv, aes.cb.iv(), AES_IV_SIZE);
    return err;
}

BL_Err_t AES_Decrypt(BL_UINT8_T *input, BL_UINT8_T *output, BL_UINT32_T size)
{
    BL_Err_t err = size % AES_IV_SIZE == 0 ? BL_EIO : BL_ENODATA;
    if (aes.cb.decrypt && err == BL_EIO)
    {
        err = aes.cb.decrypt(input, output, size, aes.key, aes.iv) == BL_TRUE 
            ? BL_OK : BL_EACCES;
    }

    return err;
}
