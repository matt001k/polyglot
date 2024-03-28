#include "aes.h"
#include "verify.h"
#include "sha256.h"
#include "systick.h"
#include "serial.h"
#include "led.h"
#include "run.h"
#include "init.h"
#include "nvm.h"
#include "wdt.h"
#include "blink.h"
#include "update.h"
#include "jump.h"
#include "hold.h"
#include "timeout.h"
#include "validator.h"
#include "buffer.h"
#include "table.h"
#include "helper.h"

#include "uECC.h"

static int RNG(uint8_t *dest, unsigned size) {
  // Use the least-significant bits from the ADC for an unconnected pin (or connected to a source of 
  // random noise). This can take a long time to generate random data if the result of analogRead(0) 
  // doesn't change very frequently.
  int count = 0;
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 8; ++i) {
        ++count;
      
      if (count == 0) {
         val = (val << 1);
      } else {
         val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }
  // NOTE: it would be a good idea to hash the resulting random data using SHA-256 or similar.
  return 1;
}

int main(void)
{
    BL_Err_t err = BL_OK;
    BL_BOOL_T hold = BL_FALSE;

    /* Initialize Abstract */
    AES_Init();
    Verify_Init();
    SHA256_Init();
    Init_Init();
    Systick_Init();
    Serial_Init();
    NVM_Init();
    LED_Init();
    Jump_Init();
    Hold_Init();

    uint8_t input[32] =
    {
        0x6b,
        0x40,
        0x06,
        0xe8,
        0xbb,
        0x9f,
        0x6a,
        0x1a,
        0x63,
        0xe6,
        0x9e,
        0x2e,
        0x4e,
        0x91,
        0xf7,
        0x77,
        0x36,
        0xce,
        0x59,
        0xb5,
        0x6f,
        0x05,
        0xd6,
        0xd6,
        0x57,
        0x42,
        0x92,
        0x2f,
        0x4d,
        0xb1,
        0x02,
        0x44,
    };
    uint8_t str[] = "Hello World How Are You Today";
    uint8_t signature[64] = 
    {
0
    };
    uint8_t priv[] =
    {
        0xb7, 0xfd, 0x71, 0x54, 0xd5, 0x76, 0x7b, 0x18, 0x4d, 0xa2, 0xcf, 0xfd, 0x7b, 0x24, 0x79, 
        0x37, 0xd6, 0x8d, 0x6b, 0x19, 0x10, 0xdd, 0xfb, 0xdd, 0x88, 0x70, 0x2a, 0xa1, 0xd8, 0x9c, 
        0xd8, 0xa4
    };
    
    //uint8_t this[] = "T1XthRIP0B8b7RMXDJkF/V9olQ9Y0gMArUsMx/m4dfpoWKOttg4yBanYm+AvfgDOf0vgIELt7bLdBOria6OMWIn6suv/eL1yn44m9hk5cNObclSwbhsiPeRINrmACMnzrfVk0l2XC2aazF30lUpG32kxW35tWJ/Ge3MNGffYwaZxx3XIr6vDas8kYq7oNPIDQQ0lY0pFnXJmg2C1EIBzbXus9VVqtbAgRJSMKoYeNJFPsBgVwISuA8VeRmIJ7VaUgdMC8fNCJL/r0ycrMhMMDKsnHN+kk9X0jw6SDMjUjUF6jzda3Zf8ZX91UE86tq/UtWQPcb63ygvwb5sE5KtQrA==";
    uint8_t digest[32] = {0};
    static uint8_t output[1024] = {0};
    const struct uECC_Curve_t *curve =  uECC_secp256k1();

    AES_SetKey();
    AES_SetIV();
    AES_Decrypt(input, output, sizeof(input)/2);
    AES_Decrypt(input+16, (output+16), sizeof(input)/2);
    SHA256_Calculate(str, sizeof(str)-1);
    SHA256_Finish(digest);
    uECC_set_rng(RNG);
    uECC_sign(priv, digest, 32, signature, curve);
    Verify_GetKey();
    Verify_Decrypt(digest, signature);

    Table_Partition_t partition = {0};
    /* Initialize Interfaces */
    Table_Init();
    Table_ReadPartiton(PARTITION_CURRENT, &partition);
    BL_MemCpy(partition.signature, signature, sizeof(signature));
    partition.length = 1024;
    partition.crc = 0x6969;
    Table_WritePartiton(PARTITION_CURRENT, &partition);

    /* Initialize Tasks */
    Blink_Init();
    Update_Init();
    Timeout_Init();
    
    /* Determine if we should jump to application */
    Hold_Get(&hold);
    err = Validator_Run(Buffer_Get(), BL_BUFFER_SIZE);
    if (hold == BL_FALSE && err == BL_OK)
    {
        Jump_ToApp();
    }

    /* Initialize watchdog and run tasks */
    WDT_Init();
    while(1)
    {
        Run();
        WDT_Kick();
    }
}
