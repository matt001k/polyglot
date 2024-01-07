/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __TERMINAL_H
#define __TERMINAL_H

/**
 * @addtogroup terminal
 * @{
 */

/**************************************************************************//**
 * @file        terminal.h
 *
 * @brief       Terminal interface for the application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include "utility.h"

class Terminal
{
public:
    Terminal(void);
    ~Terminal();
    void Run(void);
private:
    typedef enum
    {
        INIT,
        COMMAND_INPUT,
        COMMAND_HANDLE
    } State_e;
    typedef enum
    {
        EXIT = 0U,
        CONTINUE,
        TERMINATE,
    } Action_e;
    typedef enum
    {
        NONE = -1,
        BLTEST = 0U,
        NUM_MODES,
    } Mode_e;
    typedef struct
    {
        const std::string print;
        std::function<Action_e(void)> cb;
    } App_t;
    App_t m_App[NUM_MODES + 1U];
    std::map<Mode_e, App_t> m_Apps;
    void Menu_Helper(const std::string header,
                     std::vector<std::string> opts);
    void Init(void);
    bool_t Handle(State_e &state,
                  Mode_e mode);
    std::int32_t Input(void);
    Action_e Exit(void);

    typedef enum
    {
        BL_TEST_INIT = -1,
        BL_TEST_COMMAND,
        BL_TEST_DATA,
        BL_TEST_CRC_OFFSET,
        BL_TEST_EXIT,
    } BL_Test_States_e;
    Action_e BLTest(void);
};

/**@} terminal */

#endif // __TERMINAL_H
