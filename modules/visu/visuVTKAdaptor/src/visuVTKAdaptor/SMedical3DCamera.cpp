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

#include "visuVTKAdaptor/SMedical3DCamera.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMedical3DCamera, ::fwData::Object );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType SET_AXIAL_SLOT    = "setAxial";
static const ::fwCom::Slots::SlotKeyType SET_SAGITTAL_SLOT = "setSagittal";
static const ::fwCom::Slots::SlotKeyType SET_FRONTAL_SLOT  = "setFrontal";

std::map< std::string, ::fwDataTools::helper::MedicalImage::Orientation >
SMedical3DCamera::m_orientationConversion = { { std::string("axial"), Orientation::Z_AXIS },
                                              { std::string("frontal"), Orientation::Y_AXIS },
                                              { std::string("sagittal"), Orientation::X_AXIS }};

//------------------------------------------------------------------------------

SMedical3DCamera::SMedical3DCamera() noexcept :
    m_resetAtStart(false)

{
    newSlot(SET_AXIAL_SLOT, &SMedical3DCamera::setAxialView, this);
    newSlot(SET_SAGITTAL_SLOT, &SMedical3DCamera::setSagittalView, this);
    newSlot(SET_FRONTAL_SLOT, &SMedical3DCamera::setFrontalView, this);
}

//------------------------------------------------------------------------------

SMedical3DCamera::~SMedical3DCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SMedical3DCamera::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child(
        "config.<xmlattr>");

    const std::string orientation = config.get<std::string>("sliceIndex",
                                                            "axial");
    SLM_ASSERT("Unknown orientation",
               m_orientationConversion.find(
                   orientation) != m_orientationConversion.end());
    m_helper.setOrientation(m_orientationConversion[orientation]);

    const std::string reset =
        config.get<std::string>("resetAtStart", "no");
    SLM_ASSERT("'resetAtStart' value must be 'yes' or 'no'",
               reset == "yes" || reset == "no");
    m_resetAtStart = (reset == "yes");
}

//------------------------------------------------------------------------------

void SMedical3DCamera::starting()
{
    this->initialize();

    m_camera = this->getRenderer()->GetActiveCamera();

    if(m_resetAtStart)
    {
        this->updateView();
    }
}

//------------------------------------------------------------------------------

void SMedical3DCamera::updating()
{
    this->updateView();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMedical3DCamera::stopping()
{
}

//------------------------------------------------------------------------------

void SMedical3DCamera::setSagittalView()
{
    m_helper.setOrientation(Orientation::X_AXIS);
    this->updating();
}

//------------------------------------------------------------------------------

void SMedical3DCamera::setFrontalView()
{
    m_helper.setOrientation(Orientation::Y_AXIS);
    this->updating();
}

//------------------------------------------------------------------------------

void SMedical3DCamera::setAxialView()
{
    m_helper.setOrientation(Orientation::Z_AXIS);
    this->updating();
}

//------------------------------------------------------------------------------

void SMedical3DCamera::updateView()
{
    if(m_helper.getOrientation() == Orientation::Z_AXIS )
    {
        this->resetAxialView();
    }
    else if(m_helper.getOrientation() == Orientation::Y_AXIS )
    {
        this->resetFrontalView();
    }
    else if(m_helper.getOrientation() == Orientation::X_AXIS )
    {
        this->resetSagittalView();
    }
}

//------------------------------------------------------------------------------

void SMedical3DCamera::resetSagittalView()
{
    m_camera->SetPosition(-1, 0, 0);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, 0, 1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SMedical3DCamera::resetFrontalView()
{
    m_camera->SetPosition(0, -1, 0);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, 0, 1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------

void SMedical3DCamera::resetAxialView()
{
    m_camera->SetPosition(0, 0, -1);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, -1, 0);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();
}
//------------------------------------------------------------------------------
} //namespace visuVTKAdaptor
