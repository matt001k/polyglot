/**
 * @file        crypto.c
 *
 * @brief       Provides the API AES-CBC Decryption
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-11
 */

/*****************************************************************************/
#include "crypto.h"
#include <string.h>
#include "mbedtls/sha256.h"
#include "uECC.h"

#define CRYPTO_AES_BLOCKSIZE (16)

static inline void write_RegBurst(volatile uint32_t *reg,
                                  const uint32_t *val);
static inline void read_RegBurst(volatile uint32_t *reg,
                                 uint32_t *val);

static struct
{
    mbedtls_sha256_context ctx;
} sha256;

static struct
{
    const struct uECC_Curve_t *curve;
} ecc;

void Crypto_Init(void)
{
    mbedtls_sha256_init(&sha256.ctx);
}

uint8_t *Crypto_AESKey(void)
{

    static const uint8_t key[16] =
    {
        0x30,
        0x31,
        0x30,
        0x32,
        0x30,
        0x33,
        0x30,
        0x34,
        0x30,
        0x31,
        0x30,
        0x32,
        0x30,
        0x33,
        0x30,
        0x34,
    };
    return (uint8_t *) key;
}

uint8_t *Crypto_AESIV(void)
{
    static uint8_t iv[16] = 
    {
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
        0x46,
    };
    return iv;
}

bool Crypto_AESDecrypt(uint8_t *input,
                       uint8_t *output,
                       uint32_t size,
                       uint8_t *key,
                       uint8_t *iv)
{
    uint32_t *u32Key = (uint32_t *) key;
    size /= CRYPTO_AES_BLOCKSIZE;

    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_CRYPTO0;
    /* Clear registers */
    CRYPTO0->CTRL = 0;
    CRYPTO0->WAC = 0;
    CRYPTO0->SEQCTRL = 0;
    CRYPTO0->SEQCTRLB = 0;

    /* Set Up 128bit encryption */
    CRYPTO0->CTRL &= ~CRYPTO_CTRL_AES;
    CRYPTO0->KEYBUF = u32Key[0];
    CRYPTO0->KEYBUF = u32Key[1];
    CRYPTO0->KEYBUF = u32Key[2];
    CRYPTO0->KEYBUF = u32Key[3];
    /* Fake encryption to setup key register */
    CRYPTO0->SEQ0 = CRYPTO_CMD_INSTR_AESENC << _CRYPTO_SEQ0_INSTR0_SHIFT |
        CRYPTO_CMD_INSTR_DDATA1TODDATA4 << _CRYPTO_SEQ0_INSTR1_SHIFT |
        CRYPTO_CMD_INSTR_EXEC << _CRYPTO_SEQ0_INSTR2_SHIFT;

    /* Set initialization vector */
    write_RegBurst(&CRYPTO0->DATA2, (uint32_t *) iv);

    /* Set up AES-CBC command sequence */
    CRYPTO0->SEQ0 =
            CRYPTO_CMD_INSTR_DATA1TODATA0 << _CRYPTO_SEQ0_INSTR0_SHIFT |
            CRYPTO_CMD_INSTR_AESDEC << _CRYPTO_SEQ0_INSTR1_SHIFT |
            CRYPTO_CMD_INSTR_DATA2TODATA0XOR << _CRYPTO_SEQ0_INSTR2_SHIFT |
            CRYPTO_CMD_INSTR_DATA1TODATA2 << _CRYPTO_SEQ0_INSTR3_SHIFT;
    CRYPTO0->SEQ1 = 0;

    /* Set up blocksize to run command sequence on */
    CRYPTO0->SEQCTRL = CRYPTO_AES_BLOCKSIZE << _CRYPTO_SEQCTRL_LENGTHA_SHIFT;

    while (size--)
    {
        memcpy(iv, input, CRYPTO_AES_BLOCKSIZE);
        
        /* input to data register and start command then read output */
        write_RegBurst(&CRYPTO0->DATA1, (uint32_t *) input);
        CRYPTO0->CMD = CRYPTO_CMD_SEQSTART;
        while (CRYPTO0->STATUS & CRYPTO_STATUS_SEQRUNNING) {};
        read_RegBurst(&CRYPTO0->DATA0, (uint32_t *) output);
        input += CRYPTO_AES_BLOCKSIZE;
        output += CRYPTO_AES_BLOCKSIZE;
    }
    CMU->HFBUSCLKEN0 &= ~CMU_HFBUSCLKEN0_CRYPTO0;
    return true;
}

void Crypto_SHA256Start(void)
{
    mbedtls_sha256_starts(&sha256.ctx, 0);
}

bool Crypto_SHA256Update(uint8_t *data, uint32_t size)
{
    mbedtls_sha256_update(&sha256.ctx, data, size);
    return true;
}

bool Crypto_SHA256Finish(uint8_t *digest)
{
    mbedtls_sha256_finish(&sha256.ctx, digest);
    return true;
}

uint8_t *Crypto_ECDHKey(void)
{
    uint8_t *ret = NULL;
    static const uint8_t key[] =
    {
        0x3b,0x8d,0xc1,0x9a,0xd1,0xaa,0xa8,0x74,0x1b,0x4b,0xff,0x8d,0xbe,0xdd,
        0xbc,0x3d,0x24,0xb4,0x84,0x84,0xec,0x26,0xf3,0x14,0x95,0xe2,0x90,0xdf,0x9d,
        0xd4,0xd7,0x08,0xa7,0x40,0xe8,0x2e,0x5c,0x73,0x80,0x3a,0x4e,0x01,0xc1,0xe5,
        0xba,0xc7,0x6b,0x09,0xe3,0xfa,0x92,0x97,0x1d,0x4f,0xd3,0x16,0x05,0xdb,0xaf,
        0x26,0x02,0x42,0x4e,0x7c
    };
    ecc.curve = uECC_secp256k1();
    if (uECC_valid_public_key(key, ecc.curve))
    {
        ret = (uint8_t *) key;
    }
    return (uint8_t *) ret;
}

bool Crypto_ECDHVerify(uint8_t *hash,
                       uint8_t *signature,
                       uint8_t *key)
{
    bool ret = uECC_verify(key, hash, 32, signature, ecc.curve);
    return ret;
}

static inline void write_RegBurst(volatile uint32_t *reg,
                                  const uint32_t *val)
{
    register uint32_t v0 = val[0];
    register uint32_t v1 = val[1];
    register uint32_t v2 = val[2];
    register uint32_t v3 = val[3];
    *reg = v0;
    *reg = v1;
    *reg = v2;
    *reg = v3;
}

static inline void read_RegBurst(volatile uint32_t *reg,
                                 uint32_t *val)
{
    register uint32_t v0 = *reg;
    register uint32_t v1 = *reg;
    register uint32_t v2 = *reg;
    register uint32_t v3 = *reg;
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
}

