/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup clock
 * @{
 */

/**
 * @file        clock.h
 *
 * @brief       Provides the API for Main Clock Configuration
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-01
 */

/*****************************************************************************/
#include "clock.h"
#include "platform.h"

#ifndef XTAL_ENABLE
#define XTAL_ENABLE 1
#endif

#define CLOCK_DEFAULT_VAL CLOCK_SPEED_19MHZ
#define EXTERNAL_CRYSTAL_TUNE_VAL (10U)

#define CLOCK_SPEED_4MHZ (4000000UL)
#define CLOCK_SPEED_7MHZ (7000000UL)
#define CLOCK_SPEED_13MHZ (13000000UL)
#define CLOCK_SPEED_16MHZ (16000000UL)
#define CLOCK_SPEED_19MHZ (19000000UL)
#define CLOCK_SPEED_26MHZ (26000000UL)
#define CLOCK_SPEED_32MHZ (32000000UL)
#define CLOCK_SPEED_38MHZ (38000000UL)
#define CLOCK_SPEED_48MHZ (48000000UL)

static uint32_t fSystemClock = CLOCK_DEFAULT_VAL;

void Clock_Init(uint32_t freq)
{
    /* Unlock the CMU register to write to the clock registers */
    CMU->LOCK = CMU_LOCK_LOCKKEY_UNLOCK;

    /* Enables HFPERCLK and allows LE peripherals to work above 32MHz */
    CMU->CTRL |= CMU_CTRL_HFPERCLKEN | CMU_CTRL_WSHFLE;

    /* Enables Low Frequency Internal Oscillator */
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
    while(!(CMU->STATUS & CMU_STATUS_LFRCORDY)) {};

#if XTAL_ENABLE
    /* Configure External Crystal tuning/timeout values, auto peak detection */
    CMU->HFXOCTRL = CMU_HFXOCTRL_MODE_XTAL | CMU_HFXOCTRL_LFTIMEOUT_16CYCLES |
                    CMU_HFXOCTRL_PEAKDETMODE_AUTOCMD;
    CMU->HFXOCTRL1 = CMU_HFXOCTRL1_PEAKDETTHR_THR2;
    CMU->HFXOSTARTUPCTRL = CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT |
                           EXTERNAL_CRYSTAL_TUNE_VAL << 
                           _CMU_HFXOSTARTUPCTRL_CTUNE_SHIFT;
    CMU->HFXOSTEADYSTATECTRL = CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT |
                           EXTERNAL_CRYSTAL_TUNE_VAL << 
                           _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT |
                           CMU_HFXOSTEADYSTATECTRL_PEAKDETEN |
                           CMU_HFXOSTEADYSTATECTRL_PEAKMONEN;
    CMU->HFXOTIMEOUTCTRL = CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_128KCYCLES |
                           CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_128KCYCLES |
                           CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_128KCYCLES;
    while(CMU->SYNCBUSY & CMU_SYNCBUSY_HFXOBSY);

    /* Enables Low Frequency E Internal Peripheral Clock */
    CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_LFRCO;

    /* Enables High Frequency External Oscillator */
    CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN;
    while(!(CMU->STATUS & CMU_STATUS_HFXOENS)) {};

    /* Algorithm to tune External Oscillator peak detection threshold */
    while((CMU->STATUS & CMU_STATUS_HFXOAMPLOW) &&
         (CMU->HFXOCTRL1 < CMU_HFXOCTRL1_PEAKDETTHR_THR7))
    {
        CMU->HFXOCTRL1 += 1U << _CMU_HFXOCTRL1_PEAKDETTHR_SHIFT;
    };

    while(!(CMU->STATUS & CMU_STATUS_HFXORDY)) {};

    /* Starts a single peak detection */
    CMU->CMD = CMU_CMD_HFXOPEAKDETSTART;
    while(!(CMU->STATUS & CMU_STATUS_HFXOPEAKDETRDY)) {};

    /* Sets main clock as external oscillator */
    CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFXO;
    while(!(CMU->HFCLKSTATUS & CMU_HFCLKSTATUS_SELECTED_HFXO)){};

    fSystemClock = freq;
#else
    /* Enables Low Frequency E Internal Peripheral Clock */
    CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_LFRCO;

    /* Decides which tuning to use for internal high speed oscillator */
    if (freq <= CLOCK_SPEED_4MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL0;
        fSystemClock = CLOCK_SPEED_4MHZ;
    }
    else if (freq <= CLOCK_SPEED_7MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL3;
        fSystemClock = CLOCK_SPEED_7MHZ;
    }
        else if (freq <= CLOCK_SPEED_13MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL6;
        fSystemClock = CLOCK_SPEED_13MHZ;
    }
    else if (freq <= CLOCK_SPEED_16MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL7;
        fSystemClock = CLOCK_SPEED_16MHZ;
    }
    else if (freq <= CLOCK_SPEED_19MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL8;
        fSystemClock = CLOCK_SPEED_19MHZ;
    }
    else if (freq <= CLOCK_SPEED_26MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL10;
        fSystemClock = CLOCK_SPEED_26MHZ;
    }
    else if (freq <= CLOCK_SPEED_32MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL11;
        fSystemClock = CLOCK_SPEED_32MHZ;
    }
    else if (freq <= CLOCK_SPEED_38MHZ)
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL12;
        fSystemClock = CLOCK_SPEED_38MHZ;
    }
    else
    {
        CMU->HFRCOCTRL = DEVINFO->HFRCOCAL13;
        fSystemClock = CLOCK_SPEED_48MHZ;

    }
#endif

    /* Enables the LE clock to access LE peripherals */
    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_LE;

    /* Enables Low Frequency A,B Internal Peripheral Clock */
    CMU->LFACLKSEL = CMU_LFACLKSEL_LFA_LFRCO;
    CMU->LFBCLKSEL = CMU_LFBCLKSEL_LFB_LFRCO;

    /* Configure HFCLKLE to have a prescale of two and HFCLK as one */
    CMU->HFPRESC = CMU_HFPRESC_HFCLKLEPRESC_DIV2 |
                   CMU_HFPRESC_PRESC_DEFAULT;

    /* Set all High Frequency Clocks with a Prescale of One */
    CMU->HFBUSPRESC = CMU_HFBUSPRESC_PRESC_DEFAULT;
    CMU->HFCOREPRESC = CMU_HFCOREPRESC_PRESC_DEFAULT;
    CMU->HFPERPRESC = CMU_HFPERPRESC_PRESC_DEFAULT;
    CMU->HFEXPPRESC = CMU_HFEXPPRESC_PRESC_DEFAULT;
    CMU->HFPERPRESCB = CMU_HFPERPRESCB_PRESC_DEFAULT;
    CMU->HFPERPRESCC = CMU_HFPERPRESCC_PRESC_DEFAULT;
}

void Clock_Deinit(void)
{

}

uint32_t Clock_GetFrequency(uint32_t clock)
{
    return fSystemClock;
}

void Clock_OutputInit(void)
{
    CMU->CTRL |= CMU_CTRL_CLKOUTSEL0_HFXO;
    CMU->ROUTELOC0 |= CMU_ROUTELOC0_CLKOUT0LOC_LOC5;
}

void Clock_OutputStart(void)
{
    CMU->ROUTEPEN |= CMU_ROUTEPEN_CLKOUT0PEN;
}

void Clock_OutputStop(void)
{
    CMU->ROUTEPEN &= ~(CMU_ROUTEPEN_CLKOUT0PEN);
}

/**@} clock */