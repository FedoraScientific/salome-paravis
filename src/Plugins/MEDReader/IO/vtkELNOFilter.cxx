// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "vtkELNOFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkUnstructuredGrid.h"

//vtkCxxRevisionMacro(vtkELNOFilter, "$Revision: 1.2.2.2 $");
vtkStandardNewMacro(vtkELNOFilter);

vtkELNOFilter::vtkELNOFilter()
{
  this->ShrinkFactor = 0.5;
}

vtkELNOFilter::~vtkELNOFilter()
{
}

int vtkELNOFilter::RequestData(vtkInformation *request,
    vtkInformationVector **input, vtkInformationVector *output)
{
  vtkUnstructuredGrid *usgIn = vtkUnstructuredGrid::SafeDownCast(
      input[0]->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *pdOut = vtkPolyData::SafeDownCast(
      output->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray* array = this->GetInputArrayToProcess(0, input);
  vtkIdTypeArray* offsets = vtkIdTypeArray::SafeDownCast(
      this->GetInputArrayToProcess(0, input));

  if(usgIn == NULL || offsets == NULL || pdOut == NULL)
    {
    vtkDebugMacro("vtkELNOFilter no correctly configured : offsets = " << offsets);
    return 1;
    }

  vtkInformation *info = offsets->GetInformation();
  vtkInformationQuadratureSchemeDefinitionVectorKey *key =
      vtkQuadratureSchemeDefinition::DICTIONARY();
  if(!key->Has(info))
    {
    vtkDebugMacro("Dictionary is not present in array " << offsets->GetName() << " " << offsets << " Aborting." );
    return 1;
    }

  int res = this->Superclass::RequestData(request, input, output);
  if(res == 0)
    {
    return 0;
    }

  int dictSize = key->Size(info);
  vtkQuadratureSchemeDefinition **dict =
      new vtkQuadratureSchemeDefinition *[dictSize];
  key->GetRange(info, dict, 0, 0, dictSize);

  vtkIdType ncell = usgIn->GetNumberOfCells();
  vtkPoints *points = pdOut->GetPoints();
  vtkIdType start = 0;
  for(vtkIdType cellId = 0; cellId < ncell; cellId++)
    {
    vtkIdType offset = offsets->GetValue(cellId);
    int cellType = usgIn->GetCellType(cellId);
    // a simple check to see if a scheme really exists for this cell type.
    // should not happen if the cell type has not been modified.
    if(dict[cellType] == NULL)
      continue;
    int np = dict[cellType]->GetNumberOfQuadraturePoints();
    double center[3] = {0, 0, 0};
    for(int id = start; id < start + np; id++)
      {
      double *position = points->GetPoint(id);
      center[0] += position[0];
      center[1] += position[1];
      center[2] += position[2];
      }
    center[0] /= np;
    center[1] /= np;
    center[2] /= np;
    for(int id = start; id < start + np; id++)
      {
      double *position = points->GetPoint(id);
      double newpos[3];
      newpos[0] = position[0] * this->ShrinkFactor + center[0] * (1
          - this->ShrinkFactor);
      newpos[1] = position[1] * this->ShrinkFactor + center[1] * (1
          - this->ShrinkFactor);
      newpos[2] = position[2] * this->ShrinkFactor + center[2] * (1
          - this->ShrinkFactor);
      points->SetPoint(id, newpos);
      }
    start += np;
    }

  return 1;
}

void vtkELNOFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ShrinkFactor : " << this->ShrinkFactor << endl;
}
