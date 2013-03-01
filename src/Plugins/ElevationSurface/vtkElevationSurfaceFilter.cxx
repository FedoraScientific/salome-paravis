// Copyright (C) 2010-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "vtkElevationSurfaceFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkElevationSurfaceFilter.h"
#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkGenericCell.h"
#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include <math.h>

//vtkCxxRevisionMacro(vtkElevationSurfaceFilter, "$Revision$");
vtkStandardNewMacro(vtkElevationSurfaceFilter);

vtkElevationSurfaceFilter::vtkElevationSurfaceFilter()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);

  this->ScaleFactor = 0.5;
  this->Direction[0] = 0.0;
  this->Direction[1] = 0.0;
  this->Direction[2] = 1.0;
  this->AutoDetectDirection = 1;
}

vtkElevationSurfaceFilter::~vtkElevationSurfaceFilter()
{
}

//----------------------------------------------------------------------------
int vtkElevationSurfaceFilter::ProcessRequest(vtkInformation* request,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, inputVector, outputVector);
    }

  if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
    {
    return this->RequestUpdateExtent(request, inputVector, outputVector);
    }

  // execute information
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
    {
    return this->RequestInformation(request, inputVector, outputVector);
    }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkElevationSurfaceFilter::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  // now add our info
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
  return 1;
}

//----------------------------------------------------------------------------
int vtkElevationSurfaceFilter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

//----------------------------------------------------------------------------
int vtkElevationSurfaceFilter::RequestUpdateExtent(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  int numInputPorts = this->GetNumberOfInputPorts();
  for (int i=0; i<numInputPorts; i++)
    {
    int numInputConnections = this->GetNumberOfInputConnections(i);
    for (int j=0; j<numInputConnections; j++)
      {
      vtkInformation* inputInfo = inputVector[i]->GetInformationObject(j);
      inputInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);
      }
    }
  return 1;
}

int vtkElevationSurfaceFilter::RequestInformation(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output)
{
  return 1;
}

int vtkElevationSurfaceFilter::RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output)
{
  vtkPolyData *pdIn = vtkPolyData::SafeDownCast(
      input[0]->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkUnstructuredGrid *usgOut = vtkUnstructuredGrid::SafeDownCast(
      output->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray* array = this->GetInputArrayToProcess(0, input);

  if(pdIn == NULL || array == NULL || usgOut == NULL
     || array->GetNumberOfComponents() != 1)
    {
    vtkDebugMacro("vtkElevationSurfaceFilter no correctly configured");
    return 1;
    }

  double dir[3];
  if(this->AutoDetectDirection)
    {
    this->ComputeDirection(pdIn, dir);
    }
  else
    {
    dir[0] = this->Direction[0];
    dir[1] = this->Direction[1];
    dir[2] = this->Direction[2];
    }

  double len = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];

  if(len == 0)
    {
    dir[2] = 1;
    len = 1.0;
    }

  len = sqrt(len);

  dir[0] /= len;
  dir[1] /= len;
  dir[2] /= len;

  dir[0] *= this->GetScaleFactor();
  dir[1] *= this->GetScaleFactor();
  dir[2] *= this->GetScaleFactor();

  usgOut->Allocate(pdIn->GetNumberOfCells());

  vtkSmartPointer<vtkPoints> newPts = vtkSmartPointer<vtkPoints>::New();
  usgOut->SetPoints(newPts);

  usgOut->GetPointData()->CopyAllocate(pdIn->GetPointData(),
                                       2*pdIn->GetNumberOfPoints());
  usgOut->GetCellData()->CopyAllocate(pdIn->GetCellData(),
                                       pdIn->GetNumberOfCells());

  vtkIdType ncell = pdIn->GetNumberOfCells();
  vtkSmartPointer<vtkIdList> newIds = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> polyhedronIds = vtkSmartPointer<vtkIdList>::New();
  for(vtkIdType cellId=0; cellId < ncell; cellId++)
    {
    vtkCell* cell = pdIn->GetCell(cellId);
    if(cell->GetCellDimension() != 2)
      continue;

    unsigned char newCellType = VTK_EMPTY_CELL;
    unsigned char oldCellType = cell->GetCellType();
    switch(oldCellType)
      {
      case VTK_TRIANGLE :
        newCellType = VTK_WEDGE;
        break;
      case VTK_QUAD :
        newCellType = VTK_HEXAHEDRON;
        break;
      case VTK_POLYGON :
        newCellType = VTK_POLYHEDRON;
      // default : add new cell types to extrude here
      }
    if(newCellType == VTK_EMPTY_CELL)
      continue;

    double cellScalar = array->GetTuple1(cellId);

    vtkIdList* oldIds = cell->GetPointIds();
    int oldPtsNumber = oldIds->GetNumberOfIds();
    int newPtsNumber = oldPtsNumber * 2;
    newIds->SetNumberOfIds(newPtsNumber);
    double coords[VTK_CELL_SIZE*3];
    for(int ptid = 0; ptid < oldPtsNumber; ptid++)
      {
      pdIn->GetPoint(oldIds->GetId(ptid), coords + 3*ptid);
      }
    for(int ptid = 0; ptid < oldPtsNumber; ptid++)
      {
      coords[(ptid+oldPtsNumber)*3+0] = coords[ptid*3+0] + cellScalar*dir[0];
      coords[(ptid+oldPtsNumber)*3+1] = coords[ptid*3+1] + cellScalar*dir[1];
      coords[(ptid+oldPtsNumber)*3+2] = coords[ptid*3+2] + cellScalar*dir[2];
      }
    for(int ptid=0; ptid<newPtsNumber; ptid++)
      {
      vtkIdType newId = newPts->InsertNextPoint(coords + 3*ptid);
      newIds->SetId(ptid, newId);
      usgOut->GetPointData()->CopyData(pdIn->GetPointData(),
                                       oldIds->GetId(ptid % oldPtsNumber),
                                       newIds->GetId(ptid));
      }
    vtkIdType newCellId;
    if(newCellType == VTK_POLYHEDRON)
      {
      polyhedronIds->Initialize();
      // in the polyhedron case, I will generate a quad for each edge
      // of the input, and two capping faces
      polyhedronIds->InsertNextId(2+oldPtsNumber);
      // insert the bottom face
      polyhedronIds->InsertNextId(oldPtsNumber);
      for(int ptid = 0; ptid < oldPtsNumber; ptid++)
        {
        polyhedronIds->InsertNextId(newIds->GetId(ptid));
        }
      // insert the top face
      polyhedronIds->InsertNextId(oldPtsNumber);
      for(int ptid = oldPtsNumber; ptid < 2*oldPtsNumber; ptid++)
        {
        polyhedronIds->InsertNextId(newIds->GetId(ptid));
        }
      // insert the bording quads
      for(int qid = 0; qid < oldPtsNumber; qid++)
        {
        polyhedronIds->InsertNextId(4);
        polyhedronIds->InsertNextId(newIds->GetId(qid));
        polyhedronIds->InsertNextId(newIds->GetId(qid+oldPtsNumber));
        polyhedronIds->InsertNextId(newIds->GetId(qid+((oldPtsNumber+1)%oldPtsNumber)));
        polyhedronIds->InsertNextId(newIds->GetId((qid+1)%oldPtsNumber));
        }
      newIds->Initialize();
      for(int jj=0; jj<polyhedronIds->GetNumberOfIds(); jj++)
        {
        newIds->InsertNextId(polyhedronIds->GetId(jj));
        }
      }
    newCellId = usgOut->InsertNextCell(newCellType, newIds);
    usgOut->GetCellData()->CopyData(pdIn->GetCellData(),
                                   cellId,
                                   newCellId);
    }

  usgOut->GetFieldData()->ShallowCopy(pdIn->GetFieldData());

  usgOut->Squeeze();

  return 1;
}

void  vtkElevationSurfaceFilter::ComputeDirection(vtkPolyData* pdIn, double *outDir)
{
  double tmp[2][3] = {{0, 0, 0}, {0, 0, 0}};
  outDir[0] = outDir[1] = outDir[2] = 0;

  vtkPoints* pts = pdIn->GetPoints();
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();

  for(vtkIdType cellId = 0; cellId < pdIn->GetNumberOfCells(); cellId++)
    {
    pdIn->GetCell(cellId, cell);
    if(cell->GetCellDimension() != 2)
      continue;

    vtkIdList* ptIds = cell->GetPointIds();
    for(int i=0; i<ptIds->GetNumberOfIds(); i++)
      {
      vtkIdType firstId = ptIds->GetId(i);
      vtkIdType secondId = ptIds->GetId((i+1)%ptIds->GetNumberOfIds());
      pts->GetPoint(firstId, tmp[0]);
      pts->GetPoint(secondId, tmp[1]);
      outDir[0] += tmp[0][1]*tmp[1][2] - tmp[0][2]*tmp[1][1];
      outDir[1] += tmp[0][2]*tmp[1][0] - tmp[0][0]*tmp[1][2];
      outDir[2] += tmp[0][0]*tmp[1][1] - tmp[0][1]*tmp[1][0];
      }
    }
}

void vtkElevationSurfaceFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ScaleFactor : " << this->ScaleFactor << endl;
}
