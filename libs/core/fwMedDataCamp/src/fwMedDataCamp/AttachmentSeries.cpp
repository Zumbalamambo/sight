/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwMedDataCamp/AttachmentSeries.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMemory/BufferObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(AttachmentSeries))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("attachment_availability", &::fwMedData::AttachmentSeries::m_attachmentAvailability)
    .property("attachment_path", &::fwMedData::AttachmentSeries::m_attachmentPath)
    .property("media_type", &::fwMedData::AttachmentSeries::m_mediaType)
    .property("buffer", &::fwMedData::AttachmentSeries::m_buffer)
    ;
}

fwCampImplementEnumMacro((fwMedData)(AttachmentSeries)(AttachmentAvailability))
{
    builder
    .value("NONE",  ::fwMedData::AttachmentSeries::NONE)
    .value("LOCAL",  ::fwMedData::AttachmentSeries::LOCAL)
    .value("EMBEDDED",  ::fwMedData::AttachmentSeries::EMBEDDED)
    ;
}
