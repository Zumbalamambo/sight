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

#include "DefaultDicomFilterCustomTest.hpp"

#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <filesystem>
#include <gdcmScanner.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::DefaultDicomFilterCustomTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void DefaultDicomFilterCustomTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_SEG";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    // Read DicomSeries
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::custom::DefaultDicomFilter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesContainer.size());
    ::fwMedData::DicomSeries::sptr dicomSeriesA = dicomSeriesContainer[0];
    ::fwMedData::DicomSeries::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check SOP Class UIDs
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.2"), *dicomSeriesA->getSOPClassUIDs().begin());       // CT
                                                                                                                    // Image
                                                                                                                    // Storage
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.66.5"), *dicomSeriesB->getSOPClassUIDs().begin());    // Surface
                                                                                                                    // Segmentation
                                                                                                                    // Storage
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
