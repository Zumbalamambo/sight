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

#pragma once

#include "uiMeasurement/config.hpp"

#include <fwCore/macros.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action adds distances on an image field.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiMeasurement::action::SAddDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: Image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API SAddDistance final : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(SAddDistance,  ::fwGui::IActionSrv)

    /// Creates the service.
    UIMEASUREMENT_API SAddDistance() noexcept;

    /// Destroys the service.
    UIMEASUREMENT_API virtual ~SAddDistance() noexcept override final;

private:

    /// Configures the action.
    virtual void configuring() override final;

    /// Starts the action.
    virtual void starting() override final;

    /// Adds a new distance to the image field.
    virtual void updating() override final;

    /// Stops the action.
    virtual void stopping() override final;

};

} // namespace action

} // namespace uiMeasurement
