#include "aes.h"
#include "blink.h"
#include "ecc.h"
#include "hold.h"
#include "init.h"
#include "jump.h"
#include "led.h"
#include "nvm.h"
#include "run.h"
#include "serial.h"
#include "sha256.h"
#include "systick.h"
#include "table.h"
#include "timeout.h"
#include "update.h"
#include "wdt.h"

int main(void)
{
  BL_Err_t  err  = BL_ERR;
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

  /* Initialize Interfaces */
  Table_Init();

  /* Initialize Tasks */
  Blink_Init();
  Update_Init();
  Timeout_Init();

  /* Determine if we should jump to application */
  Hold_Get(&hold);
  if(hold == BL_FALSE && err == BL_OK) {
    Jump_ToApp();
  }

  /* Initialize watchdog and run tasks */
  WDT_Init();
  while(1) {
    Run();
    WDT_Kick();
  }
}
