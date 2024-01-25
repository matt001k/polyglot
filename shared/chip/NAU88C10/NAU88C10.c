/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup NAU88C10
 * @{
 */

/**************************************************************************//**
 * @file        NAU88C10.c
 *
 * @brief       Driver for NAU88C10 IC
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-26
 *****************************************************************************/
#include "NAU88C10.h"
#include "NAU88C10_Config.h"

#define I2C_CHECK(inst, reg, val, err)          \
do                                              \
{                                               \
    if (err == NAU88C10_OK)                     \
    {                                           \
        err = i2c_Write(inst, reg, val, true);  \
    }                                           \
} while (0)

static NAU88C10_Err_t i2c_Write(NAU88C10_t *inst,
                                uint8_t address,
                                uint16_t data,
                                bool validate);
static NAU88C10_Err_t i2c_Read(NAU88C10_t *inst,
                               uint8_t address,
                               uint16_t *data);

NAU88C10_Err_t NAU88C10_Init(NAU88C10_t *inst, NAU88C10_Cfg_t cfg)
{
    NAU88C10_Err_t err = NAU88C10_OK;
    if (inst)
    {
        inst->cfg = cfg;

        err = NAU88C10_Reset(inst);
        /* Power Management Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_POWER_MANAGEMENT_1,
                  NAU88C10_CFG_POWER_MANAGEMENT_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_POWER_MANAGEMENT_2,
                  NAU88C10_CFG_POWER_MANAGEMENT_2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_POWER_MANAGEMENT_3,
                  NAU88C10_CFG_POWER_MANAGEMENT_3, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_POWER_MANAGEMENT_4,
                  NAU88C10_CFG_POWER_MANAGEMENT_4, err);

        /* PLL Configuration For 12MHz Clock and 48kHz sample rate */
        I2C_CHECK(inst, NAU88C10_REGISTER_PLL_N_CTRL,
                  NAU88C10_CFG_PLL_N_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_PLL_K_1,
                  NAU88C10_CFG_PLL_K_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_PLL_K_2,
                  NAU88C10_CFG_PLL_K_2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_PLL_K_3,
                  NAU88C10_CFG_PLL_K_3, err);

        /* Audio Control Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_AUDIO_INTERFACE,
                  NAU88C10_CFG_AUDIO_INTERFACE, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_COMPANDING,
                  NAU88C10_CFG_COMPANDING, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_CLOCK_CONTROL_1,
                  NAU88C10_CFG_CLOCK_CONTROL_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_CLOCK_CONTROL_2,
                  NAU88C10_CFG_CLOCK_CONTROL_2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_DAC_CTRL,
                  NAU88C10_CFG_DAC_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_DAC_VOLUME,
                  NAU88C10_CFG_DAC_VOLUME, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ADC_CTRL,
                  NAU88C10_CFG_ADC_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ADC_VOLUME,
                  NAU88C10_CFG_ADC_VOLUME, err);

        /* Equalizer Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_EQ1_LOW_CUTOFF,
                  NAU88C10_CFG_EQ1_LOW_CUTOFF, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_EQ2_PEAK1,
                  NAU88C10_CFG_EQ2_PEAK1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_EQ3_PEAK2,
                  NAU88C10_CFG_EQ3_PEAK2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_EQ4_PEAK3,
                  NAU88C10_CFG_EQ4_PEAK3, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_EQ5_HIGH_CUTOFF,
                  NAU88C10_CFG_EQ5_HIGH_CUTOFF, err);

        /* DAC Limiter Configuration Register */
        I2C_CHECK(inst, NAU88C10_REGISTER_DAC_LIMITER_1,
                  NAU88C10_CFG_DAC_LIMITER_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_DAC_LIMITER_2,
                  NAU88C10_CFG_DAC_LIMITER_2, err);

        /* ADC Notch Filter Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_NOTCH_FILTER_HIGH_1,
                  NAU88C10_CFG_NOTCH_FILTER_HIGH_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_NOTCH_FILTER_LOW_1,
                  NAU88C10_CFG_NOTCH_FILTER_LOW_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_NOTCH_FILTER_HIGH_2,
                  NAU88C10_CFG_NOTCH_FILTER_HIGH_2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_NOTCH_FILTER_LOW_2,
                  NAU88C10_CFG_NOTCH_FILTER_LOW_2, err);

        /* ALC Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_ALC_CTRL_1,
                  NAU88C10_CFG_ALC_CTRL_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ALC_CTRL_2,
                  NAU88C10_CFG_ALC_CTRL_2, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ALC_CTRL_3,
                  NAU88C10_CFG_ALC_CTRL_3, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_NOISE_GATE,
                  NAU88C10_CFG_NOISE_GATE, err);

        /* Input, Output and Mixer Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_ATTENUATION_CTRL,
                  NAU88C10_CFG_ATTENUATION_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_INPUT_CTRL,
                  NAU88C10_CFG_INPUT_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_PGA_GAIN,
                  NAU88C10_CFG_PGA_GAIN, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ADC_BOOST,
                  NAU88C10_CFG_ADC_BOOST, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_OUTPUT_CTRL,
                  NAU88C10_CFG_OUTPUT_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_MIXER_CTRL,
                  NAU88C10_CFG_MIXER_CTRL, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_SPKOUT_VOLUME,
                  NAU88C10_CFG_SPKOUT_VOLUME, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_MONO_MIXER_CONTROL,
                  NAU88C10_CFG_MONO_MIXER_CONTROL, err);

        /* PCM Time Slot and ADCOOUT Impedence Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_TIME_SLOT,
                  NAU88C10_CFG_TIME_SLOT, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ADCOUT_DRIVE,
                  NAU88C10_CFG_ADCOUT_DRIVE, err);

        /* Output Driver Control Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_HIGH_VOLTAGE_CTRL,
                  NAU88C10_CFG_HIGH_VOLTAGE_CTRL, err);

        /* ALC Enhanced Configuration Registers */
        I2C_CHECK(inst, NAU88C10_REGISTER_ALC_ENHANCEMENTS_1,
                  NAU88C10_CFG_ALC_ENHANCEMENTS_1, err);
        I2C_CHECK(inst, NAU88C10_REGISTER_ALC_ENHANCEMENTS_2,
                  NAU88C10_CFG_ALC_ENHANCEMENTS_2, err);

        /* Misc Control Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_ADDITIONAL_IF_CTRL,
                  NAU88C10_CFG_ADDITIONAL_IF_CTRL, err);

        /* Output Tie-Off Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_POWER_TIE_OFF_CTRL,
                  NAU88C10_CFG_POWER_TIE_OFF_CTRL, err);

        /* AGC Peak-To-Peak Out Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_AGC_P2P_DETECTOR,
                  NAU88C10_CFG_AGC_P2P_DETECTOR, err);

        /* AGC Peak Out Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_PEAK_DETECTOR,
                  NAU88C10_CFG_PEAK_DETECTOR, err);

        /* Automute Control and Status Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_CONTROL_AND_STATUS,
                  NAU88C10_CFG_CONTROL_AND_STATUS, err);

        /* Output Tie-Off Direct Manual Control Register Configuration */
        I2C_CHECK(inst, NAU88C10_REGISTER_OUTPUT_TIE_OFF_CTRL,
                  NAU88C10_CFG_OUTPUT_TIE_OFF_CTRL, err);
    }
    return err;
}

NAU88C10_Err_t NAU88C10_Reset(NAU88C10_t *inst)
{
    NAU88C10_Err_t err = NAU88C10_ERR;
    err = i2c_Write(inst, NAU88C10_REGISTER_SOFTWARE_RESET, 0x0001, false);
    return err;
}


static NAU88C10_Err_t i2c_Write(NAU88C10_t *inst,
                                uint8_t address,
                                uint16_t data,
                                bool validate)
{
    NAU88C10_Err_t err = NAU88C10_ERR;
    uint8_t reg[sizeof(data)] =
    {
        address << 1 | (uint8_t) (data >> 8U),
        (uint8_t) data,
    };
#ifdef NAU88C10_VERIFY_WRITE
    uint16_t compare = (uint16_t) (~((reg[0] << 8U) & 0x01) | ~(reg[1]));
#endif //NAU88C10_VERIFY_WRITE

    if (inst->cfg.write)
    {
        inst->cfg.write(NAU88C10_ADDRESS, reg, sizeof(data), true);
#ifdef NAU88C10_VERIFY_WRITE
        if (validate)
        {
            err = i2c_Read(inst, address, &compare);
            if (err != NAU88C10_OK || (uint8_t) compare != reg[1] ||
                (uint8_t) (compare >> 8U) != (reg[0] & 0x01))
            {
               err = NAU88C10_ENODEV;
            }
        }
        else
        {
            err = NAU88C10_OK;
        }
#else
        err = NAU88C10_OK;
#endif //NAU88C10_VERIFY_WRITE
    }
    return err;
}

static NAU88C10_Err_t i2c_Read(NAU88C10_t *inst,
                               uint8_t address,
                               uint16_t *data)
{
    NAU88C10_Err_t err = NAU88C10_ERR;
    uint8_t reg[sizeof(*data)] =
    {
        address << 1U,
        0U,
    };
    if (inst->cfg.read && inst->cfg.write)
    {
        err = NAU88C10_OK;
        inst->cfg.write(NAU88C10_ADDRESS, reg, sizeof(address), false);
        inst->cfg.read(NAU88C10_ADDRESS, reg, sizeof(*data), true);
        *data = (uint16_t) (((reg[0] << 8U) & 0x01) | reg[1]);
    }
    return err;
}

/**@} NAU88C10 */
