/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_COMMAND_H
#define __BL_COMMAND_H

/**
 * @addtogroup command
 * @{
 */

/**************************************************************************//**
 * @file        command.h
 *
 * @brief       Provides and interface for the necessary commands needed for
 *              the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include <iostream>
#include <map>
#include "common.h"
#include "serial.h"
#include "dict.h"

class Command
{
public:
    typedef enum
    {
        RECEIVE_READY = 0U,
        RECEIVE_ERROR,
        RECEIVE_NUM_COMMAND,
    } Command_Receive_e;
    typedef enum
    {
        TRANSMIT_READY = 0U,
        TRANSMIT_ERROR,
        TRANSMIT_WRITE,
        TRANSMIT_RUN,
        TRANSMIT_VALIDATE,
        TRANSMIT_ERASE,
        TRANSMIT_LOCK,
        TRANSMIT_UNLOCK,
        TRANSMIT_RELEASE,
        TRANSMIT_RESET,
        TRANSMIT_NUM_COMMAND,
    } Command_Transmit_e;
    Command();
    ~Command();
    BL_Err_t Send(Serial serial, Command_Transmit_e cmd);
    BL_Err_t Receive(Serial serial,
                     Dict_Item_t *dict,
                     Command_Receive_e *cmd);

private:
    typedef std::map<Command_Receive_e, Dict_Item_t> Rx_Map_t;
    typedef std::map<Command_Transmit_e, Dict_Item_t> Tx_Map_t;
    Tx_Map_t m_TxMap;
    Rx_Map_t m_RxMap;
};

/**@} command */

#endif // __BL_COMMAND_H