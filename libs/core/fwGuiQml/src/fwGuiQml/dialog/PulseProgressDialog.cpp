/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/PulseProgressDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QFutureWatcher>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QString>
#include <QtConcurrent>
#include <QtCore>

fwGuiRegisterMacro(::fwGuiQml::dialog::PulseProgressDialog, ::fwGui::dialog::IPulseProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog(::fwGui::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

PulseProgressDialog::~PulseProgressDialog()
{
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/PulseProgressDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("pulseProgressDialog", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialogPath, context);
    SLM_ASSERT("The Qml File PulseProgressDialog is not found or not loaded", window);
    // keep window to destroy it

    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;

    window->setProperty("title", m_title);
    QObject* dialog = window->findChild<QObject*>("dialog");
    SLM_ASSERT("The dialog is not found inside the window", dialog);
    Q_EMIT messageChanged();
    // Start the computation.

    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    QObject::connect(&futureWatcher, SIGNAL(finished()), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    futureWatcher.setFuture(QtConcurrent::run(m_stuff));
    qGuiApp->installEventFilter(this);
    loop.exec();
    qGuiApp->removeEventFilter(this);
    delete window;
}

//------------------------------------------------------------------------------

bool PulseProgressDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Shortcut || event->type() == QEvent::ShortcutOverride)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
