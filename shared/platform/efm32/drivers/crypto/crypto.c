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

#define CRYPTO_AES_BLOCKSIZE (16)

static inline void write_RegBurst(volatile uint32_t *reg,
                                  const uint32_t *val);
static inline void read_RegBurst(volatile uint32_t *reg,
                                 uint32_t *val);

void Crypto_AESInit(void)
{
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
