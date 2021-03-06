/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "monitorQt/config.hpp"

#include <fwCom/Connection.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwMemory/BufferManager.hpp>

#include <fwTools/Failed.hpp>

#include <QFutureWatcher>
#include <QPointer>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableView>
#include <QTableWidget>

#include <vector>

class QTimer;

namespace fwCom
{
template< typename F >
struct Slot;
}

namespace monitorQt
{

/**
 * @brief   Editor to dump or restore selected buffer.
 */
class MONITORQT_CLASS_API DumpEditor : public QObject,
                                       public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(DumpEditor, ::fwGui::editor::IEditor);

    /// Constructor. Does nothing.
    MONITORQT_API DumpEditor() noexcept;

    /// Destructor. Does nothing.
    MONITORQT_API virtual ~DumpEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout and call updating() method
    virtual void starting() override;

    /// Stop the layout.
    virtual void stopping() override;

    /// Update the choice selection
    virtual void updating() override;

    /// Call updating() method
    virtual void swapping() override;

    /// Calls classic IAction methods to configure
    virtual void configuring() override;

    /// Overrides. Does nothing.
    virtual void info( std::ostream& _sstream ) override;

    /// Start m_updateTimer, call on buffManager signal emit ( see m_refreshSignal )
    void onUpdate();

protected Q_SLOTS:

    /// This method is called when an item is pressed.
    void changeStatus(int);

    /// Slot called when user click on button m_refresh, call updating() method
    void onRefreshButton();

    void onBufferInfo();

private:

    typedef ::fwCom::Slot<void ()> UpdateSlotType;

    QFutureWatcher< ::fwMemory::BufferManager::BufferInfoMapType > m_watcher;

    // Managed buffers
    std::vector< const void* const* > m_objectsUID;

    /// Widget to print some information on managed buffer by system
    QTableWidget* m_list;

    /// Button to force refresh
    QPushButton*  m_refresh;

    /// Mapper use in list widget to map each dump button to an action
    QSignalMapper* m_mapper;

    /// Editor to manage dump policy
    QTableView* m_policyEditor;

    /// Editor to show few memory information
    QTableView* m_infoEditor;

    SPTR(UpdateSlotType) m_updateSlot;

    /// Manage connection between buffManager updated signal and onUpdate method
    ::fwCom::Connection m_connection;

    /// Timer use to call each 300 ms onRefreshButton() slot
    QPointer<QTimer> m_updateTimer;
};

} // monitorQt
