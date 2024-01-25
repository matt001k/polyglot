/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup NAU88C10
 * @{
 */

/**************************************************************************//**
 * @file        NAU88C10.h
 *
 * @brief       Driver for NAU88C10 IC
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-26
 *****************************************************************************/
#ifndef __NAU88C10_H
#define __NAU88C10_H

#include "platform.h"

#ifndef NAU88C10_VERIFY_WRITE
#define NAU88C10_VERIFY_WRITE 1
#endif //NAU88C10_VERIFY_WRITE

#define NAU88C10_ADDRESS 0b00110100

#define NAU88C10_REGISTER_SOFTWARE_RESET (0x00)
#define NAU88C10_REGISTER_POWER_MANAGEMENT_1 (0x01)
#define NAU88C10_REGISTER_POWER_MANAGEMENT_2 (0x02)
#define NAU88C10_REGISTER_POWER_MANAGEMENT_3 (0x03)
#define NAU88C10_REGISTER_AUDIO_INTERFACE (0x04)
#define NAU88C10_REGISTER_COMPANDING (0x05)
#define NAU88C10_REGISTER_CLOCK_CONTROL_1 (0x06)
#define NAU88C10_REGISTER_CLOCK_CONTROL_2 (0x07)
#define NAU88C10_REGISTER_DAC_CTRL (0x0A)
#define NAU88C10_REGISTER_DAC_VOLUME (0x0B)
#define NAU88C10_REGISTER_ADC_CTRL (0x0E)
#define NAU88C10_REGISTER_ADC_VOLUME (0x0F)
#define NAU88C10_REGISTER_EQ1_LOW_CUTOFF (0x12)
#define NAU88C10_REGISTER_EQ2_PEAK1 (0x13)
#define NAU88C10_REGISTER_EQ3_PEAK2 (0x14)
#define NAU88C10_REGISTER_EQ4_PEAK3 (0x15)
#define NAU88C10_REGISTER_EQ5_HIGH_CUTOFF (0x16)
#define NAU88C10_REGISTER_DAC_LIMITER_1 (0x18)
#define NAU88C10_REGISTER_DAC_LIMITER_2 (0x19)
#define NAU88C10_REGISTER_NOTCH_FILTER_HIGH_1 (0x1B)
#define NAU88C10_REGISTER_NOTCH_FILTER_LOW_1 (0x1C)
#define NAU88C10_REGISTER_NOTCH_FILTER_HIGH_2 (0x1D)
#define NAU88C10_REGISTER_NOTCH_FILTER_LOW_2 (0x1E)
#define NAU88C10_REGISTER_ALC_CTRL_1 (0x20)
#define NAU88C10_REGISTER_ALC_CTRL_2 (0x21)
#define NAU88C10_REGISTER_ALC_CTRL_3 (0x22)
#define NAU88C10_REGISTER_NOISE_GATE (0x23)
#define NAU88C10_REGISTER_PLL_N_CTRL (0x24)
#define NAU88C10_REGISTER_PLL_K_1 (0x25)
#define NAU88C10_REGISTER_PLL_K_2 (0x26)
#define NAU88C10_REGISTER_PLL_K_3 (0x27)
#define NAU88C10_REGISTER_ATTENUATION_CTRL (0x28)
#define NAU88C10_REGISTER_INPUT_CTRL (0x2C)
#define NAU88C10_REGISTER_PGA_GAIN (0x2D)
#define NAU88C10_REGISTER_ADC_BOOST (0x2F)
#define NAU88C10_REGISTER_OUTPUT_CTRL (0x31)
#define NAU88C10_REGISTER_MIXER_CTRL (0x32)
#define NAU88C10_REGISTER_SPKOUT_VOLUME (0x36)
#define NAU88C10_REGISTER_MONO_MIXER_CONTROL (0x38)
#define NAU88C10_REGISTER_POWER_MANAGEMENT_4 (0x3A)
#define NAU88C10_REGISTER_TIME_SLOT (0x3B)
#define NAU88C10_REGISTER_ADCOUT_DRIVE (0x3C)
#define NAU88C10_REGISTER_SILICON_REVISION (0x3E)
#define NAU88C10_REGISTER_2_WIRE_ID (0x3F)
#define NAU88C10_REGISTER_ADDITIONAL_ID (0x40)
#define NAU88C10_REGISTER_RESERVED (0x41)
#define NAU88C10_REGISTER_HIGH_VOLTAGE_CTRL (0x45)
#define NAU88C10_REGISTER_ALC_ENHANCEMENTS_1 (0x46)
#define NAU88C10_REGISTER_ALC_ENHANCEMENTS_2 (0x47)
#define NAU88C10_REGISTER_ADDITIONAL_IF_CTRL (0x49)
#define NAU88C10_REGISTER_POWER_TIE_OFF_CTRL (0x4B)
#define NAU88C10_REGISTER_AGC_P2P_DETECTOR (0x4C)
#define NAU88C10_REGISTER_PEAK_DETECTOR (0x4D)
#define NAU88C10_REGISTER_CONTROL_AND_STATUS (0x4E)
#define NAU88C10_REGISTER_OUTPUT_TIE_OFF_CTRL (0x4F)

typedef enum
{
    NAU88C10_OK = 0,
    NAU88C10_ERR = 1,
    NAU88C10_EIO = 5,
    NAU88C10_ENXIO = 6,
    NAU88C10_ENODEV = 19,
} NAU88C10_Err_t;

typedef void (*NAU88C10_Write_t)(uint8_t address,
                                 uint8_t *data,
                                 uint8_t size,
                                 bool stop);

typedef void (*NAU88C10_Read_t)(uint8_t address,
                                uint8_t *data,
                                uint8_t size,
                                bool stop);

typedef struct
{
    NAU88C10_Write_t write;
    NAU88C10_Read_t read;
} NAU88C10_Cfg_t;


typedef struct
{
    NAU88C10_Cfg_t cfg;
} NAU88C10_t;


NAU88C10_Err_t NAU88C10_Init(NAU88C10_t *inst, NAU88C10_Cfg_t cfg);
NAU88C10_Err_t NAU88C10_Reset(NAU88C10_t *inst);


#endif // __NAU88C10_H

/**@} NAU88C10 */
