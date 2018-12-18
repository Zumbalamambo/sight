/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

namespace itkRegistrationOp
{
namespace ut
{

class AutomaticRegistrationTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( AutomaticRegistrationTest );
CPPUNIT_TEST( identityTest );
CPPUNIT_TEST( rigidTransformTest );
CPPUNIT_TEST( translateTransformTest );
CPPUNIT_TEST( rotationTransformTest );
CPPUNIT_TEST( multiresolutionRotationTransformTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void identityTest();
    void rigidTransformTest();
    void translateTransformTest();
    void rotationTransformTest();
    void multiresolutionRotationTransformTest();

};

} //namespace ut
} //namespace itkRegistrationOp