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

#include "DataCampHelper.hpp"

#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/getObject.hpp>

#include <camp/class.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace DataCampHelper
{

//------------------------------------------------------------------------------

void visitProperties(const std::string& className, const PropertiesNameType& vecProp)
{
    const ::camp::Class& metaClass = ::camp::classByName(className);
    CPPUNIT_ASSERT_EQUAL( vecProp.size(), metaClass.propertyCount());
    for(const std::string& property :  vecProp)
    {
        CPPUNIT_ASSERT_MESSAGE("Missing property "+ property+" in "+className,
                               metaClass.hasProperty(property));
    }
}

//------------------------------------------------------------------------------

void compareSimplePropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                const std::string& value)
{
    ::fwData::GenericFieldBase::sptr field;
    field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, field);
    CPPUNIT_ASSERT_EQUAL( value, field->toString());
}

//------------------------------------------------------------------------------

void compareObjectPropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                ::fwData::Object::sptr value)
{
    ::fwData::Object::sptr subObj;
    subObj = ::fwDataCamp::getObject(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, subObj);
    CPPUNIT_ASSERT_MESSAGE("Retrieve property "+propertyPath+" not equal with value", value == subObj);
}

//------------------------------------------------------------------------------

} // end namespace DataCampHelper
