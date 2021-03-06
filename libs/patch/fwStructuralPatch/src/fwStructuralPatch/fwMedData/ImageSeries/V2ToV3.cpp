/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwStructuralPatch/fwMedData/ImageSeries/V2ToV3.hpp"

#include <fwAtoms/String.hpp>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace ImageSeries
{

V2ToV3::V2ToV3() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwMedData::ImageSeries";
    m_targetClassname = "::fwMedData::ImageSeries";
    m_originVersion   = "2";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3(const V2ToV3& _cpy) :
    ::fwAtomsPatch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply( const ::fwAtoms::Object::sptr& _previous,
                    const ::fwAtoms::Object::sptr& _current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions)
{
    IStructuralPatch::apply(_previous, _current, _newVersions);

    // Updates object version.
    this->updateVersion(_current);

    ::fwAtomsPatch::helper::Object helper(_current);
    // We add Series attributes since the patcher can't retrieve the mother class of ImageSeries.
    // The patcher can't call mother class patch.
    helper.addAttribute("number", ::fwAtoms::String::New(""));
    helper.addAttribute("laterality", ::fwAtoms::String::New(""));
    helper.addAttribute("protocolName", ::fwAtoms::String::New(""));
    helper.addAttribute("body_part_examined", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_position", ::fwAtoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", ::fwAtoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", ::fwAtoms::String::New(""));

    helper.addAttribute("contrast_bolus_agent", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_route", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_volume", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_start_time", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_stop_time", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_total_dose", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_rate", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_duration", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient", ::fwAtoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient_concentration", ::fwAtoms::String::New(""));
    helper.addAttribute("acquisition_date", ::fwAtoms::String::New(""));
    helper.addAttribute("acquisition_time", ::fwAtoms::String::New(""));
}

} // namespace ImageSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
