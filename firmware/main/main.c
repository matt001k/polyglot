#include "aes.h"
#include "ecc.h"
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
#include "table.h"

int main(void)
{
    BL_Err_t err = BL_ERR;
    BL_BOOL_T hold = BL_FALSE;

    /* Initialize Abstract */
    AES_Init();
    ECC_Init();
    SHA256_Init();
    Init_Init();
    Systick_Init();
    Serial_Init();
    NVM_Init();
    LED_Init();
    Jump_Init();
    Hold_Init();

    /* Initialize Interfaces */
    Table_Init();

    /* Initialize Tasks */
    Blink_Init();
    Update_Init();
    Timeout_Init();
    
    /* Determine if we should jump to application */
    Hold_Get(&hold);
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
