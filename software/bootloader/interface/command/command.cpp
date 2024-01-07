/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup command
 * @{
 */

/**************************************************************************//**
 * @file        command.cpp
 *
 * @brief       Provides and interface for the necessary commands needed for
 *              the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include "command.h"

#define COMMAND_SIZE sizeof(Dict_Item_t)

Command::Command() :
    m_TxMap{ {TRANSMIT_READY, BL_READY},
             {TRANSMIT_ERROR, BL_ERROR},
             {TRANSMIT_WRITE, BL_WRITE},
             {TRANSMIT_RUN, BL_RUN},
             {TRANSMIT_VALIDATE, BL_VALIDATE},
             {TRANSMIT_ERASE, BL_ERASE},
             {TRANSMIT_LOCK, BL_LOCK},
             {TRANSMIT_UNLOCK, BL_UNLOCK},
             {TRANSMIT_RELEASE, BL_RELEASE_PORT},
             {TRANSMIT_RESET, BL_RESET} },
    m_RxMap{ {RECEIVE_READY, BL_READY},
             {RECEIVE_ERROR, BL_ERROR} }
{

}

Command::~Command()
{

}

BL_Err_t Command::Send(Serial serial, Command_Transmit_e cmd)
{
    BL_Err_t err = BL_EINVAL;
    std::uint8_t buf[COMMAND_SIZE] = {0U};
    std::int8_t bIdx = COMMAND_SIZE - 1U;
    Dict_Item_t tTemp = 0U;

    if (cmd < TRANSMIT_NUM_COMMAND)
    {
        err = BL_ENODEV;
        tTemp = m_TxMap[cmd];
        buf[bIdx--] = (std::uint8_t) (tTemp);
        for (; bIdx >= 0; --bIdx)
        {
            buf[bIdx] = (std::uint8_t) (tTemp >>= 8U);
        }
        err = serial.Transmit(buf, COMMAND_SIZE);
    }

    return err;
}

BL_Err_t Command::Receive(Serial serial,
                          Dict_Item_t *dict,
                          Command_Receive_e *cmd)
{
    BL_Err_t err = BL_ENODATA;
    std::uint8_t buf[COMMAND_SIZE] = {0U};

    err = serial.Receive(buf, COMMAND_SIZE);
    if (err == BL_OK)
    {
        *dict = (Dict_Item_t) (buf[0] << 24U |
                               buf[1] << 16U |
                               buf[2] << 8U |
                               buf[3]);
        err = BL_ENOMSG;

        for (uint8_t cIdx = 0U; cIdx < RECEIVE_NUM_COMMAND; cIdx++)
        {
            if (m_RxMap[(Command_Receive_e) cIdx] == *dict)
            {
                *cmd = (Command_Receive_e) cIdx;
                err = BL_OK;
                break;
            }
        }
    }

    return err;
}

/**@} command */
