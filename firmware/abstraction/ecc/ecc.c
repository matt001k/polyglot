/******************************************************************************
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/******************************************************************************
 * @file        ecc.c
 *
 * @brief       Provides an abstraction layer for verifying firmware signatures
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-24
 *****************************************************************************/
#include "ecc.h"

#include "ecdh_key.h"

#define ECC_VERIFY_CB_EXPAND(v) .verify = v

typedef BL_BOOL_T (*ECC_Cb_t)(BL_UINT8_T *hash,
                              BL_UINT8_T *signature,
                              BL_UINT8_T *key);

typedef struct {
  BL_UINT8_T *key;
  ECC_Cb_t    verify;
} ECC_t;

BL_STATIC ECC_t ecc = { NULL, ECC_CFG(ECC_VERIFY_CB_EXPAND) };

BL_Err_t ECC_Init(void)
{
  BL_Err_t err = BL_EIO;

  ecc.key = ecdh_key;
  if(ecc.verify) {
    err = BL_OK;
  }
  return err;
}

BL_Err_t ECC_Decrypt(BL_UINT8_T *hash, BL_UINT8_T *signature)
{
  BL_Err_t err = hash && signature ? BL_EIO : BL_ENODATA;
  if(err == BL_EIO) {
    err = ecc.verify(hash, signature, ecc.key) == BL_TRUE ? BL_OK : BL_ERR;
  }
  return err;
}
