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

#pragma once

#include "fwGdcmIO/container/sr/DicomSRNode.hpp"
#include "fwGdcmIO/writer/tid/TemplateID.hpp"

#include <fwData/Image.hpp>

namespace fwData
{
class PointList;
}

namespace fwGdcmIO
{
namespace writer
{
namespace tid
{

/**
 * @brief Measurement TID
 * @see TID 300
 */
class FWGDCMIO_CLASS_API Measurement : public ::fwGdcmIO::writer::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    FWGDCMIO_API Measurement(const SPTR(::gdcm::Writer)& writer,
                             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                             const ::fwData::Image::csptr& image);

    /// Destructor
    FWGDCMIO_API virtual ~Measurement();

    /**
     * @brief Create nodes according to the template
     * @param[in] parent Parent node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    FWGDCMIO_API virtual void createNodes(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                                          bool useSCoord3D = true);

protected:

    /**
     * @brief Create a measurement node
     * @param[in] parent Parent node
     * @param[in] pointList Pointlist containing distance points
     * @param[in] id ID of the ficudial
     * @param[in] useSCoord3D True if we must use 3D coordinates
     */
    void createMeasurement(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                           const CSPTR(::fwData::PointList)& pointList,
                           unsigned int id,
                           bool useSCoord3D);

};

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO
