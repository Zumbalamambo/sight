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

#include "maths/config.hpp"

#include <fwServices/IOperator.hpp>

namespace maths
{

/**
 * @brief   This service decompose a matrix into a rotation/scaling matrix and a translation matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::maths::SDecomposeMatrix">
            <in key="source" uid="..." autoConnect="yes" />
            <inout key="translation" uid="..." />
            <inout key="rotation" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: matrix to decompose.
 * @subsection In-Out In-Out:
 * - \b translation [::fwData::TransformationMatrix3D]: translation matrix.
 * - \b rotation [::fwData::TransformationMatrix3D]: rotation/scaling matrix.
 *
 */
class MATHS_CLASS_API SDecomposeMatrix : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro((SDecomposeMatrix)(::fwServices::IOperator))

    /**
     * @brief Constructor.
     */
    MATHS_API SDecomposeMatrix() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SDecomposeMatrix() noexcept
    {
    }

protected:

    /// This method is used to configure the service.
    MATHS_API void configuring() override;

    /// This method is used to initialize the service.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Does nothing.
    MATHS_API void updating() override;

    MATHS_API virtual KeyConnectionsMap getAutoConnections() const override;
};

} //namespace maths
