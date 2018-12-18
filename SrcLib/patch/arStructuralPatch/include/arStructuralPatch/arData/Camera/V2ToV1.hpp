/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __ARSTRUCTURALPATCH_ARDATA_CAMERA_V2TOV1_HPP__
#define __ARSTRUCTURALPATCH_ARDATA_CAMERA_V2TOV1_HPP__

#include "arStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

/// Structural patch to convert a arData::Camera from version '2' to version '1'.
class ARSTRUCTURALPATCH_CLASS_API V2ToV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V2ToV1)(::arStructuralPatch::arData::Camera::V2ToV1), (()), new V2ToV1);

    /// Constructor
    ARSTRUCTURALPATCH_API V2ToV1();

    /// Destructor
    ARSTRUCTURALPATCH_API ~V2ToV1();

    /// Copy constructor
    ARSTRUCTURALPATCH_API V2ToV1( const V2ToV1& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes from reconstruction.
     */
    ARSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Camera

} // namespace arData

} // namespace arStructuralPatch

#endif /* __ARSTRUCTURALPATCH_ARDATA_CAMERA_V2TOV1_HPP__ */

