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

#include "fwGdcmIO/reader/ie/Document.hpp"

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "fwGdcmIO/helper/StructuredReport.hpp"
#include "fwGdcmIO/reader/tid/MeasurementReport.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Document::Document(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                   const SPTR(::gdcm::Reader)& reader,
                   const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                   const ::fwData::Image::sptr& image,
                   const ::fwLog::Logger::sptr& logger,
                   ProgressCallback progress,
                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image,
                                                                 logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::readSR()
{
    // Retrieve dataset
    const ::gdcm::DataSet& datasetRoot = m_reader->GetFile().GetDataSet();

    // Create SR from Dataset
    SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) rootContainerNode =
        ::fwGdcmIO::helper::StructuredReport::readSR(datasetRoot);

    if(rootContainerNode)
    {
        // Try to read a measurement report
        ::fwGdcmIO::reader::tid::MeasurementReport report(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
        report.readSR(rootContainerNode);
    }

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
