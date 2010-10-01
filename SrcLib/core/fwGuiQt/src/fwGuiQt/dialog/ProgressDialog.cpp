/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwGuiQt/dialog/ProgressDialog.hpp"


REGISTER_BINDING( ::fwGui::dialog::IProgressDialog, ::fwGuiQt::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::FactoryRegistryKeyType , ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );


namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( const std::string title, const std::string message)
{
    m_pdialog = new QProgressDialog( qApp->activeWindow() );

    // FIXME modal dialog has conflict with MessageHandler
    m_pdialog->setWindowModality(Qt::NonModal);
    m_pdialog->setMinimum(0);
    m_pdialog->setMaximum(100);
    m_pdialog->setValue(0);
    m_pdialog->setCancelButton(0);

    this->setTitle(title);
    this->setMessage(message);

    m_pdialog->show();
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    m_pdialog->deleteLater();
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent,std::string msg)
{
    assert(m_pdialog);
    int value = (int)(percent*100);
    OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
    this->setMessage(msg);
    m_pdialog->setValue(value);
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string &title)
{
    m_pdialog->setWindowTitle(QString::fromStdString(title));
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string &msg)
{
    m_pdialog->setLabelText(QString::fromStdString(msg));
}

//------------------------------------------------------------------------------


} // namespace dialog
} // namespace fwGuiQt
