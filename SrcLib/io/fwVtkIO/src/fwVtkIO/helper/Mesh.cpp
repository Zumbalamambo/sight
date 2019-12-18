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

#include "fwVtkIO/helper/Mesh.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/helper/Mesh.hpp>
#include <fwDataTools/helper/MeshGetter.hpp>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkExtractUnstructuredGrid.h>
#include <vtkFillHolesFilter.h>
#include <vtkFloatArray.h>
#include <vtkGeometryFilter.h>
#include <vtkMassProperties.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>

namespace fwVtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

void Mesh::fromVTKMesh(  vtkSmartPointer<vtkPolyData> polyData, ::fwData::Mesh::sptr mesh )
{
    vtkPoints* points = polyData->GetPoints();
    if (points)
    {
        mesh->clear();
        const vtkIdType numberOfPoints = points->GetNumberOfPoints();
        const vtkIdType numberOfCells  = polyData->GetNumberOfCells();

        ::fwData::Mesh::Attributes attributes = ::fwData::Mesh::Attributes::NONE;
        vtkSmartPointer<vtkUnsignedCharArray> pointColors = nullptr;
        vtkSmartPointer<vtkUnsignedCharArray> cellColors  = nullptr;
        vtkSmartPointer<vtkFloatArray> pointNormals       = nullptr;
        vtkSmartPointer<vtkFloatArray> cellNormals        = nullptr;
        vtkSmartPointer<vtkFloatArray> pointTexCoords     = nullptr;
        vtkSmartPointer<vtkFloatArray> cellTexCoords      = nullptr;

        if(polyData->GetPointData()->HasArray("Colors"))
        {
            attributes  = attributes | ::fwData::Mesh::Attributes::POINT_COLORS;
            pointColors = vtkUnsignedCharArray::SafeDownCast(polyData->GetPointData()->GetArray("Colors"));
        }
        if(polyData->GetCellData()->HasArray("Colors"))
        {
            attributes = attributes | ::fwData::Mesh::Attributes::CELL_COLORS;
            cellColors = vtkUnsignedCharArray::SafeDownCast(polyData->GetCellData()->GetArray("Colors"));
        }
        if(polyData->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            attributes   = attributes | ::fwData::Mesh::Attributes::POINT_NORMALS;
            pointNormals = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetNormals());
        }
        if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            attributes  = attributes | ::fwData::Mesh::Attributes::CELL_NORMALS;
            cellNormals = vtkFloatArray::SafeDownCast(polyData->GetCellData()->GetNormals());
        }
        if(polyData->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            attributes     = attributes | ::fwData::Mesh::Attributes::POINT_TEX_COORDS;
            pointTexCoords = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetTCoords());
        }
        if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            attributes    = attributes | ::fwData::Mesh::Attributes::CELL_TEX_COORDS;
            cellTexCoords = vtkFloatArray::SafeDownCast(polyData->GetCellData()->GetTCoords());
        }

        mesh->reserve(static_cast< ::fwData::Mesh::Id>(numberOfPoints), static_cast< ::fwData::Mesh::Id>(numberOfCells),
                      ::fwData::Mesh::CellType::TRIANGLE, attributes);

        const auto dumpLock = mesh->lock();

        const double* point;
        mesh->setNumberOfPoints(static_cast< ::fwData::Mesh::Id>(numberOfPoints));
        auto pointIter = mesh->begin< ::fwData::iterator::PointIterator >();
        for (vtkIdType i = 0; i < numberOfPoints; ++i)
        {
            point             = points->GetPoint(i);
            *pointIter->point =
            {static_cast<float>(point[0]), static_cast<float>(point[1]), static_cast<float>(point[2])};
            if (pointColors)
            {
                if (pointColors->GetNumberOfComponents() == 3)
                {
                    const auto color = pointColors->GetPointer(i*3);
                    pointIter->rgba->r = color[0];
                    pointIter->rgba->g = color[1];
                    pointIter->rgba->b = color[2];
                    pointIter->rgba->a = 255;
                }
                else
                {
                    const auto color = pointColors->GetPointer(i*4);
                    pointIter->rgba->r = color[0];
                    pointIter->rgba->g = color[1];
                    pointIter->rgba->b = color[2];
                    pointIter->rgba->a = color[3];
                }
            }
            if (pointNormals)
            {
                const auto normal = pointNormals->GetPointer(i*3);
                pointIter->normal->nx = normal[0];
                pointIter->normal->ny = normal[1];
                pointIter->normal->nz = normal[2];
            }
            if (pointTexCoords)
            {
                const auto texCoords = pointTexCoords->GetPointer(i*2);
                pointIter->tex->u = texCoords[0];
                pointIter->tex->v = texCoords[1];
            }
            ++pointIter;
        }

        vtkCell* cell;
        vtkIdList* idList;
        int cellType;
        for (vtkIdType i = 0; i < numberOfCells; ++i)
        {
            cell     = polyData->GetCell(i);
            idList   = cell->GetPointIds();
            cellType = cell->GetCellType();

            switch (cellType)
            {
                case VTK_VERTEX:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 1);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(0)));
                    break;
                case VTK_LINE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 2);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)));
                    break;
                case VTK_TRIANGLE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 3);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(2)));
                    break;
                case VTK_QUAD:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 4);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         2)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(3)));
                    break;
                case VTK_POLY_VERTEX:
                    for(vtkIdType i = 0; i < idList->GetNumberOfIds(); ++i)
                    {
                        mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(i)));
                    }
                    break;
                default:
                    FW_RAISE("VTK Mesh type "<<cellType<< " not supported.");
            }
        }
        mesh->adjustAllocatedMemory();

        if (cellColors || cellNormals || cellTexCoords)
        {
            auto cellIter = mesh->begin< ::fwData::iterator::CellIterator >();

            for (vtkIdType i = 0; i != numberOfCells; ++i)
            {
                if (cellColors)
                {
                    if (cellColors->GetNumberOfComponents() == 3)
                    {
                        const auto color = cellColors->GetPointer(i*3);
                        cellIter->rgba->r = color[0];
                        cellIter->rgba->g = color[1];
                        cellIter->rgba->b = color[2];
                        cellIter->rgba->a = 255;
                    }
                    else
                    {
                        const auto color = cellColors->GetPointer(i*4);
                        cellIter->rgba->r = color[0];
                        cellIter->rgba->g = color[1];
                        cellIter->rgba->b = color[2];
                        cellIter->rgba->a = color[3];
                    }
                }
                if (cellNormals)
                {
                    const auto normal = cellNormals->GetPointer(i*3);
                    cellIter->normal->nx = normal[0];
                    cellIter->normal->ny = normal[1];
                    cellIter->normal->nz = normal[2];
                }
                if (cellTexCoords)
                {
                    const auto texCoords = cellTexCoords->GetPointer(i*2);
                    cellIter->tex->u = texCoords[0];
                    cellIter->tex->v = texCoords[1];
                }
                ++cellIter;
            }
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, ::fwData::Mesh::sptr mesh)
{
    vtkPoints* points = grid->GetPoints();
    if(points)
    {
        mesh->clear();
        const vtkIdType numberOfPoints = points->GetNumberOfPoints();
        const vtkIdType numberOfCells  = grid->GetNumberOfCells();

        ::fwData::Mesh::Attributes attributes = ::fwData::Mesh::Attributes::NONE;
        vtkSmartPointer<vtkUnsignedCharArray> pointColors = nullptr;
        vtkSmartPointer<vtkUnsignedCharArray> cellColors  = nullptr;
        vtkSmartPointer<vtkFloatArray> pointNormals       = nullptr;
        vtkSmartPointer<vtkFloatArray> cellNormals        = nullptr;
        vtkSmartPointer<vtkFloatArray> pointTexCoords     = nullptr;
        vtkSmartPointer<vtkFloatArray> cellTexCoords      = nullptr;

        if(grid->GetPointData()->HasArray("Colors"))
        {
            attributes  = attributes | ::fwData::Mesh::Attributes::POINT_COLORS;
            pointColors = vtkUnsignedCharArray::SafeDownCast(grid->GetPointData()->GetArray("Colors"));
        }
        if(grid->GetCellData()->HasArray("Colors"))
        {
            attributes = attributes | ::fwData::Mesh::Attributes::CELL_COLORS;
            cellColors = vtkUnsignedCharArray::SafeDownCast(grid->GetCellData()->GetArray("Colors"));
        }
        if(grid->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            attributes   = attributes | ::fwData::Mesh::Attributes::POINT_NORMALS;
            pointNormals = vtkFloatArray::SafeDownCast(grid->GetPointData()->GetNormals());
        }
        if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            attributes  = attributes | ::fwData::Mesh::Attributes::CELL_NORMALS;
            cellNormals = vtkFloatArray::SafeDownCast(grid->GetCellData()->GetNormals());
        }
        if(grid->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            attributes     = attributes | ::fwData::Mesh::Attributes::POINT_TEX_COORDS;
            pointTexCoords = vtkFloatArray::SafeDownCast(grid->GetPointData()->GetTCoords());
        }
        if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            attributes    = attributes | ::fwData::Mesh::Attributes::CELL_TEX_COORDS;
            cellTexCoords = vtkFloatArray::SafeDownCast(grid->GetCellData()->GetTCoords());
        }

        mesh->reserve(static_cast< ::fwData::Mesh::Id>(numberOfPoints), static_cast< ::fwData::Mesh::Id>(numberOfCells),
                      ::fwData::Mesh::CellType::TRIANGLE, attributes);

        const auto dumpLock = mesh->lock();

        const double* point;
        mesh->setNumberOfPoints(static_cast< ::fwData::Mesh::Id>(numberOfPoints));
        auto pointIter = mesh->begin< ::fwData::iterator::PointIterator >();
        for (vtkIdType i = 0; i < numberOfPoints; ++i)
        {
            point             = points->GetPoint(i);
            *pointIter->point =
            {static_cast<float>(point[0]), static_cast<float>(point[1]), static_cast<float>(point[2])};
            if (pointColors)
            {
                if (pointColors->GetNumberOfComponents() == 3)
                {
                    const auto color = pointColors->GetPointer(i*3);
                    pointIter->rgba->r = color[0];
                    pointIter->rgba->g = color[1];
                    pointIter->rgba->b = color[2];
                    pointIter->rgba->a = 255;
                }
                else
                {
                    const auto color = pointColors->GetPointer(i*4);
                    pointIter->rgba->r = color[0];
                    pointIter->rgba->g = color[1];
                    pointIter->rgba->b = color[2];
                    pointIter->rgba->a = color[3];
                }
            }
            if (pointNormals)
            {
                const auto normal = pointNormals->GetPointer(i*3);
                pointIter->normal->nx = normal[0];
                pointIter->normal->ny = normal[1];
                pointIter->normal->nz = normal[2];
            }
            if (pointTexCoords)
            {
                const auto texCoords = pointTexCoords->GetPointer(i*2);
                pointIter->tex->u = texCoords[0];
                pointIter->tex->v = texCoords[1];
            }
            ++pointIter;
        }

        vtkCell* cell;
        vtkIdList* idList;
        int cellType;
        for (vtkIdType i = 0; i < numberOfCells; ++i)
        {
            cell     = grid->GetCell(i);
            idList   = cell->GetPointIds();
            cellType = cell->GetCellType();

            switch (cellType)
            {
                case VTK_VERTEX:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 1);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(0)));
                    break;
                case VTK_LINE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 2);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)));
                    break;
                case VTK_TRIANGLE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 3);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(2)));
                    break;
                case VTK_QUAD:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 4);
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         2)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(3)));
                    break;
                case VTK_TETRA:
                    mesh->pushCell(static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         0)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(1)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(
                                                                         2)),
                                   static_cast< ::fwData::Mesh::Id >(idList->GetId(3)),
                                   ::fwData::Mesh::CellType::TETRA);
                    break;
                default:
                    FW_RAISE("VTK Mesh type "<<cellType<< " not supported.");
            }
        }

        mesh->adjustAllocatedMemory();

        if (cellColors || cellNormals || cellTexCoords)
        {
            auto cellIter = mesh->begin< ::fwData::iterator::CellIterator >();

            for (vtkIdType i = 0; i != numberOfCells; ++i)
            {
                if (cellColors)
                {
                    if (cellColors->GetNumberOfComponents() == 3)
                    {
                        const auto color = cellColors->GetPointer(i*3);
                        cellIter->rgba->r = color[0];
                        cellIter->rgba->g = color[1];
                        cellIter->rgba->b = color[2];
                        cellIter->rgba->a = 255;
                    }
                    else
                    {
                        const auto color = cellColors->GetPointer(i*4);
                        cellIter->rgba->r = color[0];
                        cellIter->rgba->g = color[1];
                        cellIter->rgba->b = color[2];
                        cellIter->rgba->a = color[3];
                    }
                }
                if (cellNormals)
                {
                    const auto normal = cellNormals->GetPointer(i*3);
                    cellIter->normal->nx = normal[0];
                    cellIter->normal->ny = normal[1];
                    cellIter->normal->nz = normal[2];
                }
                if (cellTexCoords)
                {
                    const auto texCoords = cellTexCoords->GetPointer(i*2);
                    cellIter->tex->u = texCoords[0];
                    cellIter->tex->v = texCoords[1];
                }
                ++cellIter;
            }
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::toVTKMesh( const ::fwData::Mesh::csptr& mesh, vtkSmartPointer<vtkPolyData> polyData)
{
    const vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    polyData->SetPoints(pts);
    Mesh::updatePolyDataPoints(polyData, mesh);

    const auto nbCells  = mesh->getNumberOfCells();
    const auto dumpLock = mesh->lock();

    vtkSmartPointer<vtkUnsignedCharArray> colors = nullptr;
    vtkSmartPointer<vtkFloatArray> normals       = nullptr;
    vtkSmartPointer<vtkFloatArray> texCoords     = nullptr;

    if (nbCells > 0)
    {
        polyData->Allocate(static_cast< int >(nbCells));
        auto itr          = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto itrEnd = mesh->end< ::fwData::iterator::ConstCellIterator >();

        unsigned char* newColors;
        float* newNormals;
        float* newTexCoords;
        int nbColorComponents = 0;

        if (mesh->hasCellColors())
        {
            nbColorComponents = itr->rgba ? 4 : 3;
            newColors         = new unsigned char[nbCells*nbColorComponents];
        }
        if (mesh->hasCellNormals())
        {
            newNormals = new float[nbCells*3];
        }
        if (mesh->hasCellTexCoords())
        {
            newTexCoords = new float[nbCells*2];
        }

        vtkIdType typeVtkCell;
        vtkIdType cell[4];

        for (vtkIdType i = 0; itr != itrEnd; ++itr, ++i)
        {
            const ::fwData::Mesh::CellTypes cellType = *itr->type;

            switch( cellType )
            {
                case ::fwData::Mesh::POINT:
                    typeVtkCell = VTK_VERTEX;
                    cell[0]     = itr->pointIdx[0];
                    polyData->InsertNextCell( typeVtkCell, 1, cell );
                    break;
                case ::fwData::Mesh::EDGE:
                    typeVtkCell = VTK_LINE;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );
                    break;
                case ::fwData::Mesh::TRIANGLE:
                    typeVtkCell = VTK_TRIANGLE;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    cell[2]     = itr->pointIdx[2];
                    polyData->InsertNextCell( typeVtkCell, 3, cell );
                    break;
                case ::fwData::Mesh::QUAD:
                    typeVtkCell = VTK_QUAD;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    cell[2]     = itr->pointIdx[2];
                    cell[3]     = itr->pointIdx[3];
                    polyData->InsertNextCell( typeVtkCell, 4, cell );
                    break;
                case ::fwData::Mesh::TETRA:
                    typeVtkCell = VTK_LINE;

                    cell[0] = itr->pointIdx[1];
                    cell[1] = itr->pointIdx[2];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );

                    cell[0] = itr->pointIdx[2];
                    cell[1] = itr->pointIdx[3];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );

                    cell[0] = itr->pointIdx[3];
                    cell[1] = itr->pointIdx[0];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );

                    cell[0] = itr->pointIdx[2];
                    cell[1] = itr->pointIdx[0];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );

                    cell[0] = itr->pointIdx[1];
                    cell[1] = itr->pointIdx[3];
                    polyData->InsertNextCell( typeVtkCell, 2, cell );
            }
            if (mesh->hasCellColors() && nbColorComponents == 3)
            {
                newColors[i*3]   = itr->rgb->r;
                newColors[i*3+1] = itr->rgb->g;
                newColors[i*3+2] = itr->rgb->b;
            }
            else if  (mesh->hasCellColors())
            {
                newColors[i*4]   = itr->rgba->r;
                newColors[i*4+1] = itr->rgba->g;
                newColors[i*4+2] = itr->rgba->b;
                newColors[i*4+3] = itr->rgba->a;
            }
            if (mesh->hasCellNormals())
            {
                newNormals[i*3]   = itr->normal->nx;
                newNormals[i*3+1] = itr->normal->ny;
                newNormals[i*3+2] = itr->normal->nz;
            }
            if (mesh->hasCellTexCoords())
            {
                newTexCoords[i*2]   = itr->tex->u;
                newTexCoords[i*2+1] = itr->tex->v;
            }
        }

        if (mesh->hasCellColors())
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(nbColorComponents);
            colors->SetName("Colors");
            colors->SetArray(newColors, nbCells*nbColorComponents, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);
            polyData->GetCellData()->SetScalars(colors);
        }
        else if (polyData->GetCellData()->HasArray("Colors"))
        {
            polyData->GetCellData()->RemoveArray("Colors");
        }

        if (mesh->hasCellNormals())
        {
            vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
            normals->SetNumberOfComponents(3);
            normals->SetArray(newNormals, nbCells * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

            polyData->GetCellData()->SetNormals(normals);
        }
        else if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            polyData->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }

        if (mesh->hasCellTexCoords())
        {
            vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
            texCoords->SetNumberOfComponents(2);
            texCoords->SetArray(newTexCoords, nbCells * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
            polyData->GetCellData()->SetTCoords(texCoords);
        }
        else if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            polyData->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }

        polyData->Modified();
    }
    Mesh::updatePolyDataPointsAndAttributes(polyData, mesh);
}

//------------------------------------------------------------------------------

void Mesh::toVTKGrid( const ::fwData::Mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid)
{
    const vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    grid->SetPoints(pts);
    Mesh::updateGridPoints(grid, mesh);

    const auto dumpLock = mesh->lock();
    const auto nbCells  = mesh->getNumberOfCells();

    vtkSmartPointer<vtkUnsignedCharArray> colors = nullptr;
    vtkSmartPointer<vtkFloatArray> normals       = nullptr;
    vtkSmartPointer<vtkFloatArray> texCoords     = nullptr;

    if (nbCells > 0)
    {
        grid->Allocate(static_cast<int>(nbCells));

        auto itr          = mesh->begin< ::fwData::iterator::ConstCellIterator >();
        const auto itrEnd = mesh->end< ::fwData::iterator::ConstCellIterator >();

        unsigned char* newColors;
        float* newNormals;
        float* newTexCoords;
        int nbColorComponents = 0;

        if (mesh->hasCellColors())
        {
            nbColorComponents = itr->rgba ? 4 : 3;
            newColors         = new unsigned char[nbCells*nbColorComponents];
        }
        if (mesh->hasCellNormals())
        {
            newNormals = new float[nbCells*3];
        }
        if (mesh->hasCellTexCoords())
        {
            newTexCoords = new float[nbCells*2];
        }

        vtkIdType typeVtkCell;
        vtkIdType cell[4];

        for (vtkIdType i = 0; itr != itrEnd; ++itr, ++i)
        {
            const ::fwData::Mesh::CellTypes cellType = *itr->type;

            switch( cellType )
            {
                case ::fwData::Mesh::POINT:
                    typeVtkCell = VTK_VERTEX;
                    cell[0]     = itr->pointIdx[0];
                    grid->InsertNextCell( typeVtkCell, 1, cell );
                    break;
                case ::fwData::Mesh::EDGE:
                    typeVtkCell = VTK_LINE;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    grid->InsertNextCell( typeVtkCell, 2, cell );
                    break;
                case ::fwData::Mesh::TRIANGLE:
                    typeVtkCell = VTK_TRIANGLE;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    cell[2]     = itr->pointIdx[2];
                    grid->InsertNextCell( typeVtkCell, 3, cell );
                    break;
                case ::fwData::Mesh::QUAD:
                    typeVtkCell = VTK_QUAD;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    cell[2]     = itr->pointIdx[2];
                    cell[3]     = itr->pointIdx[3];
                    grid->InsertNextCell( typeVtkCell, 4, cell );
                    break;
                case ::fwData::Mesh::TETRA:
                    typeVtkCell = VTK_TETRA;
                    cell[0]     = itr->pointIdx[0];
                    cell[1]     = itr->pointIdx[1];
                    cell[2]     = itr->pointIdx[2];
                    cell[3]     = itr->pointIdx[3];
                    grid->InsertNextCell( typeVtkCell, 4, cell );
            }
            if (mesh->hasCellColors() && nbColorComponents == 3)
            {
                newColors[i*3]   = itr->rgb->r;
                newColors[i*3+1] = itr->rgb->g;
                newColors[i*3+2] = itr->rgb->b;
            }
            else if  (mesh->hasCellColors())
            {
                newColors[i*4]   = itr->rgba->r;
                newColors[i*4+1] = itr->rgba->g;
                newColors[i*4+2] = itr->rgba->b;
                newColors[i*4+3] = itr->rgba->a;
            }
            if (mesh->hasCellNormals())
            {
                newNormals[i*3]   = itr->normal->nx;
                newNormals[i*3+1] = itr->normal->ny;
                newNormals[i*3+2] = itr->normal->nz;
            }
            if (mesh->hasCellTexCoords())
            {
                newTexCoords[i*2]   = itr->tex->u;
                newTexCoords[i*2+1] = itr->tex->v;
            }
        }

        if (mesh->hasCellColors())
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(nbColorComponents);
            colors->SetName("Colors");
            colors->SetArray(newColors, nbCells*nbColorComponents, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);
            grid->GetCellData()->SetScalars(colors);
        }
        else if (grid->GetCellData()->HasArray("Colors"))
        {
            grid->GetCellData()->RemoveArray("Colors");
        }

        if (mesh->hasCellNormals())
        {
            vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
            normals->SetNumberOfComponents(3);
            normals->SetArray(newNormals, nbCells * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

            grid->GetCellData()->SetNormals(normals);
        }
        else if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            grid->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }

        if (mesh->hasCellTexCoords())
        {
            vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
            texCoords->SetNumberOfComponents(2);
            texCoords->SetArray(newTexCoords, nbCells * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
            grid->GetCellData()->SetTCoords(texCoords);
        }
        else if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            grid->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }

        grid->Modified();

    }
    Mesh::updateGridPointsAndAttributes(grid, mesh);
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointsAndAttributes(vtkSmartPointer<vtkPolyData> polyDataDst,
                                             const ::fwData::Mesh::csptr& meshSrc )
{
    const auto dumplock = meshSrc->lock();

    vtkPoints* polyDataPoints = polyDataDst->GetPoints();

    auto itr          = meshSrc->begin< ::fwData::iterator::ConstPointIterator >();
    const auto itrEnd = meshSrc->end< ::fwData::iterator::ConstPointIterator >();

    const vtkIdType nbPoints = meshSrc->getNumberOfPoints();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    unsigned char* newColors;
    float* newNormals;
    float* newTexCoords;
    int nbColorComponents = 0;
    if (meshSrc->hasPointColors())
    {
        nbColorComponents = itr->rgba ? 4 : 3;
        newColors         = new unsigned char[nbPoints*nbColorComponents];
    }
    if (meshSrc->hasPointNormals())
    {
        newNormals = new float[nbPoints*3];
    }
    if (meshSrc->hasPointTexCoords())
    {
        newTexCoords = new float[nbPoints*2];
    }

    for (vtkIdType i = 0; itr != itrEnd; ++itr, ++i)
    {
        const auto point = itr->point;
        polyDataPoints->SetPoint(i, point->x, point->y, point->z);

        if (meshSrc->hasPointColors() && nbColorComponents == 3)
        {
            newColors[i*3]   = itr->rgb->r;
            newColors[i*3+1] = itr->rgb->g;
            newColors[i*3+2] = itr->rgb->b;
        }
        else if  (meshSrc->hasPointColors())
        {
            newColors[i*4]   = itr->rgba->r;
            newColors[i*4+1] = itr->rgba->g;
            newColors[i*4+2] = itr->rgba->b;
            newColors[i*4+3] = itr->rgba->a;
        }

        if (meshSrc->hasPointNormals())
        {
            newNormals[i*3]   = itr->normal->nx;
            newNormals[i*3+1] = itr->normal->ny;
            newNormals[i*3+2] = itr->normal->nz;
        }

        if (meshSrc->hasPointTexCoords())
        {
            newTexCoords[i*2]   = itr->tex->u;
            newTexCoords[i*2+1] = itr->tex->v;
        }
    }

    if (meshSrc->hasPointColors())
    {
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(nbColorComponents);
        colors->SetName("Colors");
        colors->SetArray(newColors, nbPoints*nbColorComponents, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);
        polyDataDst->GetPointData()->SetScalars(colors);
    }
    else if (polyDataDst->GetPointData()->HasArray("Colors"))
    {
        polyDataDst->GetPointData()->RemoveArray("Colors");
    }

    if (meshSrc->hasPointNormals())
    {
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        normals->SetNumberOfComponents(3);
        normals->SetArray(newNormals, nbPoints * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetNormals(normals);
    }
    else if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
    {
        polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
    }

    if (meshSrc->hasPointTexCoords())
    {
        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        texCoords->SetNumberOfComponents(2);
        texCoords->SetArray(newTexCoords, nbPoints * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        polyDataDst->GetPointData()->SetTCoords(texCoords);
    }
    else if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
    {
        polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
    }

    polyDataPoints->Modified();
    polyDataDst->Modified();
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst,
                                const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);
    ::fwDataTools::helper::ArrayGetter arrayHelper(meshSrc->getPointsArray());

    vtkPoints* polyDataPoints                    = polyDataDst->GetPoints();
    const vtkIdType nbPoints                     = meshSrc->getNumberOfPoints();
    const ::fwData::Mesh::PointValueType* points = arrayHelper.begin< ::fwData::Mesh::PointValueType >();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    for (vtkIdType i = 0; i != nbPoints; ++i)
    {
        polyDataPoints->SetPoint(i, points[i*3], points[i*3+1], points[i*3+2]);
    }

    polyDataPoints->Modified();
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                    const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointColorArray);

        const vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                          = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const unsigned char* pointColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const pointColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = pointColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(pointColor, pointColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetScalars(colors);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->HasArray("Colors"))
        {
            polyDataDst->GetPointData()->RemoveArray("Colors");
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                   const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellColorArray);

        const vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                          = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        ::fwMemory::BufferObject::Lock lock = cellColorArray->getBufferObject()->lock();
        const unsigned char* cellColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const cellColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = cellColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(cellColor, cellColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetScalars(colors);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->HasArray("Colors"))
        {
            polyDataDst->GetCellData()->RemoveArray("Colors");
        }
        polyDataDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                      const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointNormalsArray = meshSrc->getPointNormalsArray();
    if(pointNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointNormal          = arrayHelper.begin< float >();
        const float* const pointNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = pointNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(pointNormal, pointNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetNormals(normals);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                     const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellNormal          = arrayHelper.begin< float >();
        const float* const cellNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = cellNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(cellNormal, cellNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetNormals(normals);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            polyDataDst->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                        const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointTexCoordsArray = meshSrc->getPointTexCoordsArray();
    if(pointTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = pointTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointTexCoord          = arrayHelper.begin< float >();
        const float* const pointTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = pointTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(pointTexCoord, pointTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetTCoords(texCoords);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                       const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellTexCoordsArray = meshSrc->getCellTexCoordsArray();
    if(cellTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = cellTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellTexCoord          = arrayHelper.begin< float >();
        const float* const cellTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = cellTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(cellTexCoord, cellTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetTCoords(texCoords);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            polyDataDst->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        polyDataDst->Modified();
    }

}

//-----------------------------------------------------------------------------

double Mesh::computeVolume( const ::fwData::Mesh::csptr& mesh )
{
    vtkSmartPointer< vtkPolyData > vtkMeshRaw = vtkSmartPointer< vtkPolyData >::New();
    Mesh::toVTKMesh( mesh, vtkMeshRaw );

    // identify and fill holes in meshes
    vtkSmartPointer< vtkFillHolesFilter > holesFilter = vtkSmartPointer< vtkFillHolesFilter >::New();
    holesFilter->SetHoleSize(2000);
    holesFilter->SetInputData(vtkMeshRaw);
    holesFilter->Update();
    if (holesFilter->GetOutput()->GetNumberOfCells() > 0) // Filter return empty mesh when no topological holes are
                                                          // present
    {
        vtkMeshRaw = holesFilter->GetOutput();
    }

    // compute normals for polygonal mesh
    const vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInputData(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    // estimate volume, area, shape index of triangle mesh
    const vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInputConnection( filter->GetOutputPort() );
    calculator->Update();

    const double volume = calculator->GetVolume();
    OSLM_DEBUG(
        "GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " <<
            calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "volume - |volume projected| > volume/10000.0" << std::endl;
        ss << "[volume = " << volume << "]" << std::endl;
        ss << "[volume projected = " << calculator->GetVolumeProjected()<<"]";
        throw (std::out_of_range( ss.str() ));
    }

    return volume;
}
//------------------------------------------------------------------------------

void Mesh::updateGridPointsAndAttributes(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                         const ::fwData::Mesh::csptr& meshSrc )
{
    const auto dumplock = meshSrc->lock();

    vtkPoints* polyDataPoints = gridDst->GetPoints();

    auto itr          = meshSrc->begin< ::fwData::iterator::ConstPointIterator >();
    const auto itrEnd = meshSrc->end< ::fwData::iterator::ConstPointIterator >();

    const vtkIdType nbPoints = meshSrc->getNumberOfPoints();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    unsigned char* newColors;
    float* newNormals;
    float* newTexCoords;
    int nbColorComponents = 0;
    if (meshSrc->hasPointColors())
    {
        nbColorComponents = itr->rgba ? 4 : 3;
        newColors         = new unsigned char[nbPoints*nbColorComponents];
    }
    if (meshSrc->hasPointNormals())
    {
        newNormals = new float[nbPoints*3];
    }
    if (meshSrc->hasPointTexCoords())
    {
        newTexCoords = new float[nbPoints*2];
    }

    for (vtkIdType i = 0; itr != itrEnd; ++itr, ++i)
    {
        const auto point = itr->point;
        polyDataPoints->SetPoint(i, point->x, point->y, point->z);

        if (meshSrc->hasPointColors() && nbColorComponents == 3)
        {
            newColors[i*3]   = itr->rgb->r;
            newColors[i*3+1] = itr->rgb->g;
            newColors[i*3+2] = itr->rgb->b;
        }
        else if  (meshSrc->hasPointColors())
        {
            newColors[i*4]   = itr->rgba->r;
            newColors[i*4+1] = itr->rgba->g;
            newColors[i*4+2] = itr->rgba->b;
            newColors[i*4+3] = itr->rgba->a;
        }

        if (meshSrc->hasPointNormals())
        {
            newNormals[i*3]   = itr->normal->nx;
            newNormals[i*3+1] = itr->normal->ny;
            newNormals[i*3+2] = itr->normal->nz;
        }

        if (meshSrc->hasPointTexCoords())
        {
            newTexCoords[i*2]   = itr->tex->u;
            newTexCoords[i*2+1] = itr->tex->v;
        }
    }

    if (meshSrc->hasPointColors())
    {
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(nbColorComponents);
        colors->SetName("Colors");
        colors->SetArray(newColors, nbPoints*nbColorComponents, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);
        gridDst->GetPointData()->SetScalars(colors);
    }
    else if (gridDst->GetPointData()->HasArray("Colors"))
    {
        gridDst->GetPointData()->RemoveArray("Colors");
    }

    if (meshSrc->hasPointNormals())
    {
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        normals->SetNumberOfComponents(3);
        normals->SetArray(newNormals, nbPoints * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetNormals(normals);
    }
    else if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
    {
        gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
    }

    if (meshSrc->hasPointTexCoords())
    {
        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        texCoords->SetNumberOfComponents(2);
        texCoords->SetArray(newTexCoords, nbPoints * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        gridDst->GetPointData()->SetTCoords(texCoords);
    }
    else if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
    {
        gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
    }

    polyDataPoints->Modified();
    gridDst->Modified();
}

//------------------------------------------------------------------------------

void Mesh::updateGridPointNormals(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                  const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkUnstructuredGrid should not be NULL", gridDst);

    const ::fwData::Array::csptr pointNormalsArray = meshSrc->getPointNormalsArray();
    if(pointNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointNormal          = arrayHelper.begin< float >();
        const float* const pointNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = pointNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(pointNormal, pointNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetNormals(normals);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridPoints(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                            const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);
    ::fwDataTools::helper::ArrayGetter arrayHelper(meshSrc->getPointsArray());

    vtkPoints* polyDataPoints                    = gridDst->GetPoints();
    const vtkIdType nbPoints                     = meshSrc->getNumberOfPoints();
    const ::fwData::Mesh::PointValueType* points = arrayHelper.begin< ::fwData::Mesh::PointValueType >();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    for (vtkIdType i = 0; i != nbPoints; ++i)
    {
        polyDataPoints->SetPoint(i, points[i*3], points[i*3+1], points[i*3+2]);
    }

    polyDataPoints->Modified();
}

//------------------------------------------------------------------------------
void Mesh::updateGridPointColor(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                    = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const unsigned char* pointColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const pointColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = pointColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(pointColor, pointColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetScalars(colors);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->HasArray("Colors"))
        {
            gridDst->GetPointData()->RemoveArray("Colors");
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridCellColor(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                               const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                    = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const ::fwMemory::BufferObject::Lock lock = cellColorArray->getBufferObject()->lock();
        const unsigned char* cellColor            = arrayHelper.begin< unsigned char >();
        const unsigned char* const cellColorEnd   = arrayHelper.end< unsigned char >();

        const vtkIdType size = cellColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(cellColor, cellColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetScalars(colors);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->HasArray("Colors"))
        {
            gridDst->GetCellData()->RemoveArray("Colors");
        }
        gridDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updateGridCellNormals(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                 const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellNormal          = arrayHelper.begin< float >();
        const float* const cellNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = cellNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(cellNormal, cellNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetNormals(normals);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            gridDst->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridPointTexCoords(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                    const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr pointTexCoordsArray = meshSrc->getPointTexCoordsArray();
    if(pointTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = pointTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointTexCoord          = arrayHelper.begin< float >();
        const float* const pointTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = pointTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(pointTexCoord, pointTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetTCoords(texCoords);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridCellTexCoords(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                   const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellTexCoordsArray = meshSrc->getCellTexCoordsArray();

    if(cellTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                      = cellTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellTexCoord          = arrayHelper.begin< float >();
        const float* const cellTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = cellTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(cellTexCoord, cellTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetTCoords(texCoords);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            gridDst->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        gridDst->Modified();
    }

}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwVtkIO
