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

#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"

#include <vtkAssemblyPath.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCleanPolyData.h>
#include <vtkCoordinate.h>
#include <vtkCylinderSource.h>
#include <vtkFollower.h>
#include <vtkInteractorObserver.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

namespace fwRenderVTK
{

namespace vtk
{

vtkStandardNewMacro(MarkedSphereHandleRepresentation);

//----------------------------------------------------------------------
MarkedSphereHandleRepresentation::MarkedSphereHandleRepresentation() :
    vtkSphereHandleRepresentation()
{
    this->Marker = vtkCylinderSource::New();
    this->Marker->SetCenter(0., -1., 0.);
    this->Marker->SetResolution(64);
    this->Marker->SetHeight(0.);

    this->CleanPolyData = vtkCleanPolyData::New();
    this->CleanPolyData->PointMergingOn();
    this->CleanPolyData->CreateDefaultLocator();
    this->CleanPolyData->SetInputConnection(0, this->Marker->GetOutputPort(0));

    vtkPolyDataNormals* MarkerNormals = vtkPolyDataNormals::New();
    MarkerNormals->SetInputConnection( 0, this->CleanPolyData->GetOutputPort(0) );

    this->MarkerMapper = vtkPolyDataMapper::New();
    this->MarkerMapper->SetInputConnection( MarkerNormals->GetOutputPort() );
    MarkerNormals->Delete();

    this->Follower = vtkFollower::New();
    this->Follower->SetMapper(this->MarkerMapper);
    this->Follower->RotateX(90);

    // Set up the initial properties, parent's one is called in parent's constructor
    this->CreateDefaultProperties();

    this->MarkerRadiusFactor = 1.5;
    this->SetMarkerProperty(this->MarkerProperty);
    this->Marker->SetRadius(this->MarkerRadiusFactor * this->Sphere->GetRadius());
}

//----------------------------------------------------------------------
MarkedSphereHandleRepresentation::~MarkedSphereHandleRepresentation()
{
    this->Marker->Delete();
    this->CleanPolyData->Delete();
    this->MarkerMapper->Delete();
    this->Follower->Delete();
    this->MarkerProperty->Delete();
}

//-------------------------------------------------------------------------
void MarkedSphereHandleRepresentation::SetWorldPosition(double p[3])
{
    this->vtkSphereHandleRepresentation::SetWorldPosition(p);
    this->Follower->SetPosition(this->GetWorldPosition());// p may have been clamped
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::CreateDefaultProperties()
{
    this->MarkerProperty = vtkProperty::New();
    this->MarkerProperty->SetColor(1., 1., 0.);
    this->MarkerProperty->SetOpacity(0.5);
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::BuildRepresentation()
{
    if ( !this->Placed )
    {
        this->ValidPick = 1;
        this->Placed    = 1;
    }

    this->SizeBounds();
    this->Sphere->Update();

    this->Follower->SetCamera( this->GetRenderer()->GetActiveCamera() );
    this->Marker->SetRadius(this->MarkerRadiusFactor * this->Sphere->GetRadius() );
    this->Marker->Update();
    this->BuildTime.Modified();
}
//
//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::GetActors(vtkPropCollection* pc)
{
    this->Actor->GetActors(pc);
    this->Follower->GetActors(pc);
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::ReleaseGraphicsResources(vtkWindow* win)
{
    this->Actor->ReleaseGraphicsResources(win);
    this->Follower->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::RenderOpaqueGeometry(vtkViewport* viewport)
{
    this->BuildRepresentation();
    int ret = 0;
    if (this->GetRenderer()->GetActiveCamera()->GetParallelProjection())
    {
        ret = this->Follower->RenderOpaqueGeometry(viewport);
    }
    return this->Actor->RenderOpaqueGeometry(viewport) + ret;
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
    this->BuildRepresentation();
    int ret = 0;
    if (this->GetRenderer()->GetActiveCamera()->GetParallelProjection())
    {
        ret = this->Follower->RenderTranslucentPolygonalGeometry(viewport);
    }
    return this->Actor->RenderTranslucentPolygonalGeometry(viewport) + ret;
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::HasTranslucentPolygonalGeometry()
{
    return 1;
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::SetMarkerProperty(vtkProperty* p)
{
    vtkSetObjectBodyMacro(MarkerProperty, vtkProperty, p);
    if (p)
    {
        this->Follower->SetProperty( p );
    }
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkSphereHandleRepresentation::PrintSelf(os, indent);

    if ( this->MarkerProperty )
    {
        os << indent << "Selected Property: " << this->MarkerProperty << "\n";
    }
    else
    {
        os << indent << "Marker Property: (none)\n";
    }

    this->Sphere->PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------

int MarkedSphereHandleRepresentation::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
    // This line is commented to avoid vtk resetting the visibility
    //this->VisibilityOn(); //actor must be on to be picked

    vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->CursorPicker);

    if ( path != nullptr )
    {
        this->InteractionState = vtkHandleRepresentation::Selecting;
    }
    else
    {
        this->InteractionState = vtkHandleRepresentation::Outside;
        if ( this->ActiveRepresentation )
        {
            this->VisibilityOff();
        }
    }

    return this->InteractionState;
}

} // namespace vtk

} // namespace fwRenderVTK
