#include "aes.h"
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

int main(void)
{
    BL_Err_t err = BL_OK;
    BL_BOOL_T hold = BL_FALSE;

    /* Initialize Abstract */
    AES_Init();
    SHA256_Init();
    Init_Init();
    Systick_Init();
    Serial_Init();
    NVM_Init();
    LED_Init();
    Jump_Init();
    Hold_Init();

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
     uint8_t iv[16] = 
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
    uint8_t digest[64] = {0};
    static uint8_t output[1024] = {0};

    AES_SetKey(key);
    AES_SetIV(iv);
    AES_Decrypt(input, output, sizeof(input)/2);
    AES_Decrypt(input+16, (output+16), sizeof(input)/2);
    SHA256_Calculate(str, sizeof(str)-1);
    SHA256_Finish(digest);

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
