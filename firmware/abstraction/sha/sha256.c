/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        sha256.c
 *
 * @brief       Provides an abstraction layer for SHA256 hashing functions
 *              to validate signatures of incoming firmware images
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-18
 *****************************************************************************/
#include "sha256.h"
#include "config.h"

#define SHA_CB_EXPAND(start, update, finish) \
    { start, update, finish }

BL_STATIC SHA_t sha =
{
    false,
    SHA_CFG(SHA_CB_EXPAND)
};

BL_Err_t SHA256_Init(void)
{
    BL_Err_t ret = BL_EIO;
    if (sha.cb.start && sha.cb.update && sha.cb.finish)
    {
        ret = BL_OK;
    }
    return ret;
}

BL_Err_t SHA256_Calculate(BL_UINT8_T *data,
                          BL_UINT32_T size)
{
    BL_Err_t ret = BL_EIO;
    if (!sha.started)
    {
        sha.cb.start();
        sha.started = true;
    }
    if (data && size)
    {
        ret = sha.cb.update(data, size) == true ? BL_OK : BL_EACCES;
    }
    return ret;
}

BL_Err_t SHA256_Finish(BL_UINT8_T *digest)
{
    BL_Err_t ret = BL_EIO;
    if (digest)
    {
        ret = sha.cb.finish(digest) == true ? BL_OK : BL_EACCES;
    }
    return ret;
}

