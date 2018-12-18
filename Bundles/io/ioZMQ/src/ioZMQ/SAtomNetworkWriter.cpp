/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ioZMQ/SAtomNetworkWriter.hpp"

#include <fwData/Object.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwIO::IWriter, ::ioZMQ::SAtomNetworkWriter, ::fwData::Object);

namespace ioZMQ
{

//-----------------------------------------------------------------------------

SAtomNetworkWriter::SAtomNetworkWriter()
{
    FW_DEPRECATED_MSG("'ioZMQ' bundle and the associated services are no longer supported.", "19.0");
}

//-----------------------------------------------------------------------------

SAtomNetworkWriter::~SAtomNetworkWriter() noexcept
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::starting()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::configureWithIHM()
{
    ::fwGui::dialog::InputDialog inputDialog;

    inputDialog.setTitle(m_windowTitle.empty() ? "Configure server" : m_windowTitle);
    m_host = inputDialog.getInput();
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::stopping()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::updating()
{
    try
    {
        ::fwData::Object::sptr obj = this->getObject();

        m_socket = std::make_shared< ::zmqNetwork::Socket >(::zmqNetwork::Socket::Server, ::zmqNetwork::Socket::Reply);
        m_socket->start(m_host);
        std::string deviceName;
        ::fwData::Object::sptr response = m_socket->receiveObject(deviceName);
        m_socket->sendObject(obj);
        response = m_socket->receiveObject(deviceName);
        m_socket->stop();

    }
    catch (std::exception& err)
    {
        m_socket->stop();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", std::string(err.what()));
    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SAtomNetworkWriter::getIOPathType() const
{
    return ::fwIO::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ