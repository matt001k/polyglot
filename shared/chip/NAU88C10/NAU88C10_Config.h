
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup NAU88C10
 * @{
 */

/**************************************************************************//**
 * @file        NAU88C10_Config.h
 *
 * @brief       Driver Configuration File for NAU88C10 Codec
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-26
 *****************************************************************************/
#ifndef NAU88C10_CUSTOM
#ifndef __NAU88C10_CONFIG_H
#define __NAU88C10_CONFIG_H

/**************************************************************************//**
 * @name Power Management
 * @brief Configuration is as follows:
 * PLL, Mic Bias, Analog Bias Enabled, 80 Ohm Impedece VREF Selection (0x01)
 * Input Boost, PGA, ADC Enabled (0x02)
 * MOUT, MOUTMX, DAC Enable (0x03)
 * Low Power Management Cleared (0x3A)
 *****************************************************************************/
#define NAU88C10_CFG_POWER_MANAGEMENT_1 (0x0039)
#define NAU88C10_CFG_POWER_MANAGEMENT_2 (0x0015)
#define NAU88C10_CFG_POWER_MANAGEMENT_3 (0x0089)
#define NAU88C10_CFG_POWER_MANAGEMENT_4 (0x0000)

/**************************************************************************//**
 * @name PLL Configuration
 * @brief This is currently set up to prescale down to a 48.000kHz sampling
 * rate from a 12.000MHz clock
 *****************************************************************************/
#define NAU88C10_CFG_PLL_N_CTRL (0x0008)
#define NAU88C10_CFG_PLL_K_1 (0x000C)
#define NAU88C10_CFG_PLL_K_2 (0x0093)
#define NAU88C10_CFG_PLL_K_3 (0x00E9)

/**************************************************************************//**
 * @name Audio Control
 * @brief Configuration is as follows:
 * I2S Audio Interface (0x04)
 * No Companding (0x05)
 * Bit Clock Prescaled by 8, PLL Used as IMCLK, Codec in Slave Mode (0x06)
 * Audio Sample Rate of 48.000kHz Selected (0x07)
 * DAC Oversampling (0x0A)
 * DAC Max Gain (0x0B)
 * ADC Oversampling and HPF (0x0E)
 * ADC Max Gain (0x0F)
 *****************************************************************************/
#define NAU88C10_CFG_AUDIO_INTERFACE (0x0010)
#define NAU88C10_CFG_COMPANDING (0x0000)
#define NAU88C10_CFG_CLOCK_CONTROL_1 (0x010C)
#define NAU88C10_CFG_CLOCK_CONTROL_2 (0x0000)
#define NAU88C10_CFG_DAC_CTRL (0x0008)
#define NAU88C10_CFG_DAC_VOLUME (0x00FF)
#define NAU88C10_CFG_ADC_CTRL (0x0108)
#define NAU88C10_CFG_ADC_VOLUME (0x00FF)

/**************************************************************************//**
 * @name Equalizer Configuration
 * @brief Configured with the default register values 
 *****************************************************************************/
#define NAU88C10_CFG_EQ1_LOW_CUTOFF (0x012C)
#define NAU88C10_CFG_EQ2_PEAK1 (0x002C)
#define NAU88C10_CFG_EQ3_PEAK2 (0x002C)
#define NAU88C10_CFG_EQ4_PEAK3 (0x002C)
#define NAU88C10_CFG_EQ5_HIGH_CUTOFF (0x002C)

/**************************************************************************//**
 * @name DAC Limiter
 * @brief Configured with default register values, limiter is off by default 
 *****************************************************************************/
#define NAU88C10_CFG_DAC_LIMITER_1 (0x0032)
#define NAU88C10_CFG_DAC_LIMITER_2 (0x0000)

/**************************************************************************//**
 * @name ADC Notch Filter
 * @brief Configured with default register values, notch filter is off by
 * default
 *****************************************************************************/
#define NAU88C10_CFG_NOTCH_FILTER_HIGH_1 (0x0000)
#define NAU88C10_CFG_NOTCH_FILTER_LOW_1 (0x0000)
#define NAU88C10_CFG_NOTCH_FILTER_HIGH_2 (0x0000)
#define NAU88C10_CFG_NOTCH_FILTER_LOW_2 (0x0000)

/**************************************************************************//**
 * @name Automatic Level Control
 * @brief Configured with default register values, ALC/Noise Gate are
 * off by default
 *****************************************************************************/
#define NAU88C10_CFG_ALC_CTRL_1 (0x0038)
#define NAU88C10_CFG_ALC_CTRL_2 (0x000B)
#define NAU88C10_CFG_ALC_CTRL_3 (0x0032)
#define NAU88C10_CFG_NOISE_GATE (0x0000)

/**************************************************************************//**
 * @name Input, Output and Mixer
 * @brief Configuration is as follows:
 * No Speaker Attenuation (0x28)
 * Default Mic Bias if Enabled and PGA Inputs from MICP/N (0x2C)
 * No Gain for PGA (0x2D)
 * +20dB gain through input Boost (0x2F)
 * Thermal shutdown (0x31)
 * Speaker Outputs Disconnected (0x32)
 * Speaker Gain Set to Max and Change Gain Immedietly (0x36)
 * DAC to MOUT Enabled (0x38)
 *****************************************************************************/
#define NAU88C10_CFG_ATTENUATION_CTRL (0x0000)
#define NAU88C10_CFG_INPUT_CTRL (0x0003)
#define NAU88C10_CFG_PGA_GAIN (0x0010)
#define NAU88C10_CFG_ADC_BOOST (0x0100)
#define NAU88C10_CFG_OUTPUT_CTRL (0x0002)
#define NAU88C10_CFG_MIXER_CTRL (0x0000)
#define NAU88C10_CFG_SPKOUT_VOLUME (0x00BF)
#define NAU88C10_CFG_MONO_MIXER_CONTROL (0x0001)

/**************************************************************************//**
 * @name PCM Time Slot and ADCOUT Impedence Control
 * @brief Configuration is as follows:
 * PCM is Not Used, TSLOT is Set to 0 (0x3B)
 * Left and Right Channels Have the Same Data (0x3C)
 *****************************************************************************/
#define NAU88C10_CFG_TIME_SLOT (0x0000)
#define NAU88C10_CFG_ADCOUT_DRIVE (0x0004)

/**************************************************************************//**
 * @name Output Driver Control
 * @brief Configuration is as follows:
 * Cleared Register (May Need to Be Enabled If SPKOut Gain is Needed)
 *****************************************************************************/
#define NAU88C10_CFG_HIGH_VOLTAGE_CTRL (0x0000)

/**************************************************************************//**
 * @name Automatic Level Control Enhanced
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_ALC_ENHANCEMENTS_1 (0x0001)
#define NAU88C10_CFG_ALC_ENHANCEMENTS_2 (0x0000)

/**************************************************************************//**
 * @name Misc Control
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_ADDITIONAL_IF_CTRL (0x0000)

/**************************************************************************//**
 * @name Output Tie-Off
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_POWER_TIE_OFF_CTRL (0x0000)

/**************************************************************************//**
 * @name Automatic Gain Control Peak-To-Peak Out
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_AGC_P2P_DETECTOR (0x0000)

/**************************************************************************//**
 * @name Automatic Gain Control Peak Out
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_PEAK_DETECTOR (0x0000)

/**************************************************************************//**
 * @name Automute Control and Status
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_CONTROL_AND_STATUS (0x0000)

/**************************************************************************//**
 * @name Output Tie-Off Direct Manual Control
 * @brief Configured with default register values
 *****************************************************************************/
#define NAU88C10_CFG_OUTPUT_TIE_OFF_CTRL (0x0000)

#endif //__NAU88C10_CONFIG_H
#endif //NAU88C10_CUSTOM