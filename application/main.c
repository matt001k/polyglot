#include "platform.h"
#include "clock.h"
#include "cpu.h"
#include "delay.h"
#include "dma.h"
#include "flash.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "watchdog.h"
#include "bootloader.h"
#include "i2c.h"

static void uartCb(uint8_t *data, uint32_t length);
static volatile bool blSet = false;


int main(void)
{
    Watchdog_Deinit();
    CPU_Init();
    Clock_Init(12000000);
    Clock_OutputInit();
    GPIO_Init();
    Timer_Init();
    Delay_Init();
    UART_Init();
    SPI_Init();
    DMA_Init();
    I2C_Init();
    Watchdog_Init();
    Watchdog_Enable();

    GPIO_Set(FLASH_WP);

    Bootloader_Get();
    Clock_OutputStart();

    UART_RegisterRxCallback(UART_DEBUG, uartCb);


    uint8_t dataRx[6] = {0U};
    while(1)
    {
        GPIO_Toggle(LED1);
        Delay_Ms(1000U);
        GPIO_Toggle(LED2);
        if (!blSet)
        {
            Watchdog_Kick();
        }
    }
}

static void uartCb(uint8_t *data, uint32_t length)
{
    if (data && *data == (uint8_t) 's')
    {
        Bootloader_Set();
        blSet = true;
    }
}
