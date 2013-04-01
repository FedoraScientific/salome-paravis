// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

#include "vtkELNOMeshFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkUnstructuredGrid.h"
#include "vtkShrinkFilter.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"

//vtkCxxRevisionMacro(vtkELNOMeshFilter, "$Revision$")
//;
vtkStandardNewMacro(vtkELNOMeshFilter)
;

vtkELNOMeshFilter::vtkELNOMeshFilter()
{
}

vtkELNOMeshFilter::~vtkELNOMeshFilter()
{
}

int vtkELNOMeshFilter::RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output)
{
  vtkUnstructuredGrid *usgIn = vtkUnstructuredGrid::SafeDownCast(
      input[0]->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkUnstructuredGrid *usgOut = vtkUnstructuredGrid::SafeDownCast(
      output->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdTypeArray* usg_offsets = vtkIdTypeArray::SafeDownCast(
      this->GetInputArrayToProcess(0, input));

  if(usgIn == NULL || usg_offsets == NULL || usgOut == NULL)
    {
    vtkDebugMacro("vtkELNOMeshFilter no correctly configured : offsets = " << usg_offsets);
    return 1;
    }

  vtkIdTypeArray* a = vtkIdTypeArray::SafeDownCast(
      usgIn->GetCellData()->GetArray(usg_offsets->GetName()));

  vtkInformationVector *inArrayVec =
      this->Information->Get(INPUT_ARRAYS_TO_PROCESS());

  // first shrink the input
  vtkUnstructuredGrid* usgInClone = usgIn->NewInstance();

  usgInClone->ShallowCopy(usgIn);

  vtkSmartPointer<vtkShrinkFilter> shrink =
      vtkSmartPointer<vtkShrinkFilter>::New();
  shrink->SetInputData(usgInClone);
  shrink->SetShrinkFactor(0.9999);
  shrink->Update();
  vtkUnstructuredGrid* shrinked = shrink->GetOutput();

  usgInClone->Delete();

  usgOut->ShallowCopy(shrinked);

  vtkIdTypeArray* offsets = vtkIdTypeArray::SafeDownCast(
      shrinked->GetCellData()->GetArray(usg_offsets->GetName()));

  // now copy ELNO data. Start by verifying if it is possible to
  // shallow copy the array.
  vtkInformation *info = offsets->GetInformation();
  vtkInformationQuadratureSchemeDefinitionVectorKey *key =
      vtkQuadratureSchemeDefinition::DICTIONARY();
  if(!key->Has(info))
    {
    vtkDebugMacro("Dictionary is not present in array " << offsets->GetName() << " " << offsets << " Aborting." );
    return 0;
    }
  int dictSize = key->Size(info);
  vtkQuadratureSchemeDefinition **dict =
      new vtkQuadratureSchemeDefinition *[dictSize];
  key->GetRange(info, dict, 0, 0, dictSize);

  vtkIdType nVerts = shrinked->GetNumberOfPoints();
  vtkIdType ncell = usgIn->GetNumberOfCells();
  // first loop through all cells to check if a shallow copy is possible
  bool shallowok = true;
  vtkIdType previous = 0;

  for(vtkIdType cellId = 0; cellId < ncell; cellId++)
    {
    vtkIdType offset = offsets->GetValue(cellId);
    if(offset != previous)
      {
      shallowok = false;
      break;
      }
    int cellType = usgIn->GetCellType(cellId);

    if(dict[cellType] == NULL)
      {
      previous = offset;
      }
    else
      {
      previous = offset + dict[cellType]->GetNumberOfQuadraturePoints();
      }
    }
  if(previous != nVerts)
    {
    shallowok = false;
    }

  vtkFieldData* fielddata = usgIn->GetFieldData();
  for(int index = 0; index < fielddata->GetNumberOfArrays(); index++)
    {
    vtkDataArray* data = fielddata->GetArray(index);
    if(data == NULL)
      continue;

    vtkInformation* info = data->GetInformation();
    const char* arrayOffsetName = info->Get(
        vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME());

    if(arrayOffsetName == NULL ||
       strcmp(arrayOffsetName, offsets->GetName()) != 0)
      {
      shrinked->GetFieldData()->AddArray(data);
      continue;
      }

    if(shallowok)
      {
      usgOut->GetPointData()->AddArray(data);
      }
    else
      {
      vtkDataArray* newArray = data->NewInstance();
      newArray->SetName(data->GetName());
      usgOut->GetPointData()->AddArray(newArray);
      newArray->SetNumberOfComponents(data->GetNumberOfComponents());
      newArray->SetNumberOfTuples(usgOut->GetNumberOfPoints());
      newArray->CopyComponentNames(data);
      newArray->Delete();
      vtkIdList *ids = vtkIdList::New();

      for(vtkIdType cellId = 0; cellId < ncell; cellId++)
        {
        vtkIdType offset = offsets->GetValue(cellId);
        int cellType = shrinked->GetCellType(cellId);
        shrinked->GetCellPoints(cellId, ids);
        for(int id = 0; id < dict[cellType]->GetNumberOfQuadraturePoints(); id++)
          {
          const double * w = dict[cellType]->GetShapeFunctionWeights(id);
          int j;
          for(j = 0; j < dict[cellType]->GetNumberOfNodes(); j++)
            {
            if(w[j] == 1.0)
              break;
            }
          if(j == dict[cellType]->GetNumberOfNodes())
            {
            j = id;
            }
          newArray->SetTuple(ids->GetId(id), offset + j, data);
          }
        }
      ids->FastDelete();
      }
    }

  delete[] dict;

  return 1;
}

void vtkELNOMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
