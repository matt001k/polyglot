/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/
/**************************************************************************//**
 * @file        command.c
 *
 * @brief       Provides function necessary to update the device
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-30
 *****************************************************************************/
#include "updater.h"
#include "command.h"
#include "data.h"

Updater::Updater(Serial serial) :
    m_serial{serial}
{
}

Updater::~Updater()
{
}

BL_Err_t Updater::Update(std::vector<std::uint8_t> data, std::size_t transfer)
{
    BL_Err_t err = BL_OK;
    struct
    {
        size_t total;
        size_t transfer;
        Command c;
        Data d;
        Command::Command_Receive_e cmd;
        uint8_t *p;
    } update =
    {
        .total = data.size(),
        .p = static_cast<std::uint8_t*>(data.data()),
    };
    Dict_Item_t dict = 0U;
    std::cout << "Beginning Transfer..." << std::endl;

    while (update.total)
    {
        update.transfer = update.total >= transfer ? transfer : update.total;
        update.c.Send(this->m_serial, Command::TRANSMIT_WRITE);
        update.c.Receive(this->m_serial, &dict, &update.cmd);
        std::cout << "Command Received: " << update.cmd << std::endl;
        update.d.Send_Length(this->m_serial, update.transfer);
        update.c.Receive(this->m_serial, &dict, &update.cmd);
        std::cout << "Command Received: " << update.cmd << std::endl;
        std::cout << "Command Sending Data Packet of: " <<
            update.transfer << std::endl;
        update.d.Send_Data(this->m_serial, update.p, update.transfer);
        update.p += update.transfer;
        update.total -= update.transfer;
        update.c.Receive(this->m_serial, &dict, &update.cmd);
        std::cout << "Command Received: " << update.cmd << std::endl;
    }
    return err;
}
