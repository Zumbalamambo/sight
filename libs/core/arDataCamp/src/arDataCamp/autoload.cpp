/************************************************************************
 *
 * Copyright (C) 2014-2016 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include "arDataCamp/autoload.hpp"

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>


namespace arDataCamp
{

//Force link with fwDataCamp
static const int fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

//Force link with fwMedDataCamp
static const int fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;

runner runner::r;

}
