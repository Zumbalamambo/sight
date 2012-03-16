/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_FIELDTEST_HPP__
#define __FWTOOLS_UT_FIELDTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class FieldTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FieldTest );
    CPPUNIT_TEST( labelTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void labelTest();
};

} // namespace ut
} // namespace fwTools

#endif //__FWTOOLS_UT_FIELDTEST_HPP__