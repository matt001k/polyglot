/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup serial
 * @{
 */

/**************************************************************************//**
 * @file        serial.cpp
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include "serial.h"

Serial::Serial(Serial_Cfg_t cfg) :
    m_Init{cfg.init},
    m_Tx{cfg.tx},
    m_Rx{cfg.rx}
{
    if (m_Init)
    {
        m_Init();
    }
}

Serial::Serial()
{

}

Serial::~Serial()
{

}

BL_Err_t Serial::Init(Serial_Cfg_t cfg)
{
    m_Init = cfg.init;
    m_Tx = cfg.tx;
    m_Rx = cfg.rx;

    if (m_Init)
    {
        m_Init();
    }

    return BL_OK;
}

BL_Err_t Serial::Transmit(std::uint8_t *data, std::uint32_t length)
{
    BL_Err_t err = BL_ENODEV;

    if (m_Tx)
    {
        err = BL_OK;
        m_Tx(data, length);
    }

    return err;
}

BL_Err_t Serial::Receive(std::uint8_t *data, std::uint32_t length)
{
    BL_Err_t err = BL_ENODEV;

    if (m_Rx)
    {
        err = BL_OK;
        m_Rx(data, length);
    }

    return err;
}

/**@} serial */
