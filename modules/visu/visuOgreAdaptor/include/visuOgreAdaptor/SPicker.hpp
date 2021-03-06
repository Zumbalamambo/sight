/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/MeshPickerInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor performs picking queries on mouse clicks and forwards the picked data through a signal.
 *
 * See @see fwRenderOgre::interactor::MeshPickerInteractor to learn more about the interactions
 * provided by this service.
 *
 * @section Signals Signals
 * - \b picked(::fwDataTools::PickingInfo): emitted when a picking query is successful.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SPicker">
            <config layer="..." priority="0" queryMask="0xFFFFFFFF" />
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): layer on which the adaptor picks points.
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): filters out entities with mismatching flags when picking.
 */

class VISUOGREADAPTOR_CLASS_API SPicker final : public ::fwRenderOgre::IAdaptor
{
public:

    fwCoreServiceMacro(SPicker, ::fwRenderOgre::IAdaptor)

    /// Initializes the adaptor.
    VISUOGREADAPTOR_API SPicker() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API virtual ~SPicker() noexcept;

private:

    /// Configures the picker's query mask and priority.
    virtual void configuring() override;

    /// Creates the interactor and adds it to the layer.
    virtual void starting() override;

    /// Updates the service. Doesn't do anything here.
    virtual void updating() noexcept override;

    /// Destroys the interactor and removes it from the layer.
    virtual void stopping() override;

    /// Determines the execution order of the picking interactor.
    int m_priority { 2 };

    /// Defines the mask used to filter out entities when picking.
    std::uint32_t m_queryMask { 0xFFFFFFFF };

    /// Contains the interactor managed by the adaptor.
    std::shared_ptr< ::fwRenderOgre::interactor::MeshPickerInteractor > m_interactor;

    /// Defines the signal sent on picking events.
    ::fwRenderOgre::interactor::MeshPickerInteractor::PointClickedSigType::sptr m_pickedSig;

};

} // namespace visuOgreAdaptor.
