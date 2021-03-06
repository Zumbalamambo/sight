/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWZIP_UT_DIRTEST_HPP__
#define __FWZIP_UT_DIRTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwZip
{
namespace ut
{

class DirTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DirTest );
CPPUNIT_TEST( writeReadFileTest );
CPPUNIT_TEST( writeDirTest );
CPPUNIT_TEST( putFileTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadFileTest();
    void writeDirTest();
    void putFileTest();
};

} // namespace ut
} // namespace fwZip

#endif //__FWZIP_UT_DIRTEST_HPP__
