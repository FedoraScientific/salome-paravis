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

#include "vtkELNOMeshFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkInformationIntegerKey.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkUnstructuredGrid.h"
#include "vtkShrinkFilter.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkCell.h"

#include "MEDUtilities.hxx"

#include <map>

vtkStandardNewMacro(vtkELNOMeshFilter);

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

  if(usgIn == NULL || usgOut == NULL)
    {
      vtkDebugMacro("vtkELNOMeshFilter not correctly configured : Invalid input or output !");
      return 0;
    }

  // creates offsets array

  // first shrink the input
  vtkUnstructuredGrid* usgInClone = usgIn->NewInstance();
  usgInClone->ShallowCopy(usgIn);
  vtkSmartPointer<vtkShrinkFilter> shrink(vtkSmartPointer<vtkShrinkFilter>::New());
  shrink->SetInputData(usgInClone);
  shrink->SetShrinkFactor(0.9999);
  shrink->Update();
  vtkUnstructuredGrid *shrinked(shrink->GetOutput());
  usgInClone->Delete();
  usgOut->ShallowCopy(shrinked);
  // OK for the output 

  // now copy ELNO data. Start by verifying if it is possible to
  // shallow copy the array.
  vtkInformation *info(usgIn->GetInformation());
  //
  vtkIdType nVerts(shrinked->GetNumberOfPoints()),ncell(usgIn->GetNumberOfCells());
  // first loop through all cells to check if a shallow copy is possible
  bool shallowok(true);// Anthony : checks that shrink works well. Really necessary ?
  vtkIdType previous(0),offset(0);
  
  for(vtkIdType cellId = 0; cellId < ncell; cellId++)
    {
      if(offset != previous)
        {
          shallowok = false;
          break;
        }
      vtkCell *cell(usgIn->GetCell(cellId));
      vtkIdType nbptsInCell(cell->GetNumberOfPoints());
      previous = offset + nbptsInCell;
      //
      offset += nbptsInCell ;
    }
  //
  if(shallowok)
    shallowok = (previous == nVerts);
  
  vtkFieldData *fielddata(usgIn->GetFieldData());
  for(int index = 0; index < fielddata->GetNumberOfArrays(); index++)
    {
      vtkDataArray *data(fielddata->GetArray(index));
      vtkQuadratureSchemeDefinition **dict(0);
      vtkInformationQuadratureSchemeDefinitionVectorKey *key(vtkQuadratureSchemeDefinition::DICTIONARY());
      if(key->Has(data->GetInformation()))
        {
          int dictSize(key->Size(data->GetInformation()));
          dict=new vtkQuadratureSchemeDefinition *[dictSize];
          key->GetRange(data->GetInformation(),dict,0,0,dictSize);
        }
      if(data == NULL)
        continue;
      
      vtkInformation *info(data->GetInformation());
      const char *arrayOffsetName = info->Get(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME());

      bool isELGA(false);

      if(arrayOffsetName)
        {
          vtkFieldData *cellData(usgIn->GetCellData());
          vtkDataArray *offData(cellData->GetArray(arrayOffsetName));
          isELGA=offData->GetInformation()->Get(MEDUtilities::ELGA())==1;
        }

      if(arrayOffsetName == NULL || isELGA)
        {
          if(shallowok && data->GetNumberOfTuples()==nVerts )// Anthony : is it not a little confusing to assign a FieldData on Points because the number of tuples fits the number of nodes of shrinked mesh ?
            usgOut->GetPointData()->AddArray(data);
          else
            shrinked->GetFieldData()->AddArray(data);
          continue;
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
          vtkIdList *ids(vtkIdList::New());
          vtkIdType offset(0);
          for(vtkIdType cellId = 0; cellId < ncell; cellId++)
            {
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
              vtkCell *cell(usgIn->GetCell(cellId));
              vtkIdType nbptsInCell(cell->GetNumberOfPoints());
              offset+=nbptsInCell;
            }
          ids->FastDelete();
        }
      delete [] dict;
    }
  return 1;
}

void vtkELNOMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
