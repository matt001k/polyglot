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
 * @brief       Provides an interface for the bootloader to send commands to
 *              and receive commands from the device loading the new
 *              application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "config.h"

typedef enum
{
    TRANSMIT_READY = 0U,
    TRANSMIT_ERROR,
    TRANSMIT_NUM_COMMAND,
} Command_Transmit_e;

typedef enum
{
    RECEIVE_READY = 0U,
    RECEIVE_ERROR,
    RECEIVE_WRITE,
    RECEIVE_RUN,
    RECEIVE_VALIDATE,
    RECEIVE_ERASE,
    RECEIVE_LOCK,
    RECEIVE_UNLOCK,
    RECEIVE_RELEASE,
    RECEIVE_RESET,
    RECEIVE_NUM_COMMAND,
} Command_Receive_e;

/**************************************************************************//**
 * @brief Initialize the command interface
 * 
 * @details This implements a callback to determine when a command has been
 *          fully sent to the bootloader.
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Command_Init(void);

/**************************************************************************//**
 * @brief Resets the command inderface
 * 
 * @details Resets static variables in the command interface
 * 
 * @return BL_Err_t 
 *****************************************************************************/
void Command_Reset(void);

/**************************************************************************//**
 * @brief Deinitialize the command interface
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Command_Deinit(void);

/**************************************************************************//**
 * @brief Send a command over the locked serial interface
 * 
 * @param cmd[in] command to send over the serial interface
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Command_Send(Command_Transmit_e cmd);

/**************************************************************************//**
 * @brief Receive the latest command from over the locked serial interface
 * 
 * @param cmd[out] command to 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Command_Receive(Command_Receive_e *cmd);

/**@} command */

#endif //__BL_COMMAND_H