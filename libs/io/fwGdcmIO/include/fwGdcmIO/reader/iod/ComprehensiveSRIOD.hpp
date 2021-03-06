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

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief ComprehensiveSRIOD class used to read landmarks and distances stored in Enhanced Structured Report IODs
 */
class FWGDCMIO_CLASS_API ComprehensiveSRIOD : public ::fwGdcmIO::reader::iod::InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API ComprehensiveSRIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                    const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                    const ::fwLog::Logger::sptr& logger = nullptr,
                                    ProgressCallback progress           = nullptr,
                                    CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API ~ComprehensiveSRIOD();

    /** Read DICOM file
     * @throw ::fwGdcmIO::exception::Failed
     */
    FWGDCMIO_API void read(::fwMedData::Series::sptr series);

};

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
