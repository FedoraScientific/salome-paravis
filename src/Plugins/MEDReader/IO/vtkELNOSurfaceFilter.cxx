// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

#include "vtkELNOSurfaceFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPVGeometryFilter.h"
#include "vtkShrinkFilter.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"

//vtkCxxRevisionMacro(vtkELNOSurfaceFilter, "$Revision$")
//;
vtkStandardNewMacro(vtkELNOSurfaceFilter)
;

vtkELNOSurfaceFilter::vtkELNOSurfaceFilter()
{
}

vtkELNOSurfaceFilter::~vtkELNOSurfaceFilter()
{
}

int vtkELNOSurfaceFilter::RequestData(vtkInformation *request, vtkInformationVector **input, vtkInformationVector *output)
{
  vtkUnstructuredGrid *usgIn=vtkUnstructuredGrid::SafeDownCast(
      input[0]->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkUnstructuredGrid *usgOut=vtkUnstructuredGrid::SafeDownCast(
      output->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdTypeArray* usg_offsets=vtkIdTypeArray::SafeDownCast(
      this->GetInputArrayToProcess(0, input));

  if(usgIn==NULL||usg_offsets==NULL||usgOut==NULL)
    {
    vtkDebugMacro("vtkELNOSurfaceFilter no correctly configured : offsets = " << usg_offsets);
    return 1;
    }

  // first shrink the input
  vtkUnstructuredGrid* usgInClone=usgIn->NewInstance();

  usgInClone->ShallowCopy(usgIn);

  vtkSmartPointer<vtkPVGeometryFilter> geomFilter=vtkSmartPointer<
      vtkPVGeometryFilter>::New();
  geomFilter->SetInputData(usgInClone);
  geomFilter->SetPassThroughCellIds(1);
  geomFilter->SetPassThroughPointIds(1);
  geomFilter->SetUseOutline(0);
  geomFilter->Update();

  vtkPolyData* surface=vtkPolyData::SafeDownCast(geomFilter->GetOutput());
  vtkIdTypeArray* originalCellIds=vtkIdTypeArray::SafeDownCast(
      surface->GetCellData()->GetArray("vtkOriginalCellIds"));
  vtkIdTypeArray* originalPointIds=vtkIdTypeArray::SafeDownCast(
      surface->GetPointData()->GetArray("vtkOriginalPointIds"));

  if( originalCellIds == NULL )
  {
    vtkErrorMacro("vtkPVGeometryFilter return NULL 'vtkOriginalCellIds' array");
    return 0;
  }

  if(originalPointIds==NULL)
  {
    vtkErrorMacro("It appears that your dataset is not reduced using vtkPVGeometryFilter (NULL 'vtkOriginalPointIds).\n==================================================================================================\nProbably your dataset is not 3D.\nIf it is not a 3D dataset you are expected to use ELNO Mesh filter instead of ELNO Surface filter.\n==================================================================================================\n");
    return 0;
  }

  vtkSmartPointer<vtkShrinkFilter> shrink=
      vtkSmartPointer<vtkShrinkFilter>::New();
  shrink->SetInputConnection(geomFilter->GetOutputPort(0));
  shrink->SetShrinkFactor(0.9999);
  shrink->Update();

  vtkUnstructuredGrid* shrinked=shrink->GetOutput();

  usgInClone->Delete();

  usgOut->ShallowCopy(shrinked);

  vtkIdTypeArray* offsets=vtkIdTypeArray::SafeDownCast(
      shrinked->GetCellData()->GetArray(usg_offsets->GetName()));

  // now copy ELNO data. Start by verifying if it is possible to
  // shallow copy the array.
  vtkInformation *info=offsets->GetInformation();
  vtkInformationQuadratureSchemeDefinitionVectorKey *key=
      vtkQuadratureSchemeDefinition::DICTIONARY();
  if(!key->Has(info))
    {
    vtkDebugMacro("Dictionary is not present in array " << offsets->GetName()
                  << " " << offsets << " Aborting." );
    return 0;
    }
  int dictSize=key->Size(info);
  vtkQuadratureSchemeDefinition **dict=
      new vtkQuadratureSchemeDefinition *[dictSize];
  key->GetRange(info, dict, 0, 0, dictSize);

  vtkIdType ncell=shrinked->GetNumberOfCells();

  vtkFieldData* fielddata=usgIn->GetFieldData();
  vtkIdList *ids=vtkIdList::New();
  vtkIdList *surfaceIds=vtkIdList::New();
  vtkIdList *originalIds=vtkIdList::New();
  for(int index=0; index<fielddata->GetNumberOfArrays(); index++)
    {
    vtkDataArray* data=fielddata->GetArray(index);
    if(data==NULL)
      continue;

    vtkInformation* info=data->GetInformation();
    const char* arrayOffsetName=info->Get(
        vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME());

    if(arrayOffsetName == NULL ||
       strcmp(arrayOffsetName, offsets->GetName())!=0)
      {
      usgOut->GetFieldData()->AddArray(data);

      continue;
      }

    vtkDataArray* newArray=data->NewInstance();
    newArray->SetName(data->GetName());
    usgOut->GetPointData()->AddArray(newArray);
    newArray->SetNumberOfComponents(data->GetNumberOfComponents());
    newArray->SetNumberOfTuples(usgOut->GetNumberOfPoints());
    newArray->CopyComponentNames(data);
    newArray->Delete();

    for(vtkIdType cellId=0; cellId<ncell; cellId++)
      {
      vtkIdType offset=offsets->GetValue(cellId);

      vtkIdType originalCellId=originalCellIds->GetValue(cellId);
      int originalCellType=usgIn->GetCellType(originalCellId);

      shrinked->GetCellPoints(cellId, ids);
      surface->GetCellPoints(cellId, surfaceIds);

      for(int id=0; id<ids->GetNumberOfIds(); id++)
        {
        vtkIdType surfaceId=surfaceIds->GetId(id);
        vtkIdType shrinkedId=ids->GetId(id);
        vtkIdType originalPointId = originalPointIds->GetValue(surfaceId);

        usgIn->GetCellPoints(originalCellId, originalIds);
        int originalLocalId=-1;
        for(int li=0; li<originalIds->GetNumberOfIds(); li++)
          {
          if(originalPointId==originalIds->GetId(li))
            {
            originalLocalId=li;
            break;
            }
          }
        if(originalLocalId==-1)
          {
          originalLocalId=0;
          vtkErrorMacro("cannot find original id");
          }

        const double * w=dict[originalCellType]->GetShapeFunctionWeights(
            originalLocalId);
        int j;
        for(j=0; j<dict[originalCellType]->GetNumberOfNodes(); j++)
          {
          if(w[j]==1.0)
            break;
          }
        if(j==dict[originalCellType]->GetNumberOfNodes())
          {
            //vtkErrorMacro("cannot find elno weigth.");
          j=id;
          }
        newArray->SetTuple(shrinkedId, offset+j, data);
        }
      }
    }

  ids->FastDelete();
  surfaceIds->FastDelete();
  originalIds->FastDelete();
  delete[] dict;

  return 1;
}

void vtkELNOSurfaceFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
