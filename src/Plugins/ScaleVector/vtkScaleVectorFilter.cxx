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

#include "vtkScaleVectorFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"

#include <string>

using namespace std;

//vtkCxxRevisionMacro(vtkScaleVectorFilter, "$Revision$");
vtkStandardNewMacro(vtkScaleVectorFilter);

vtkScaleVectorFilter::vtkScaleVectorFilter()
{
  this->ScaleFactor = 0.5;
}

vtkScaleVectorFilter::~vtkScaleVectorFilter()
{
}

//----------------------------------------------------------------------------
int vtkScaleVectorFilter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  // now add our info
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}


int vtkScaleVectorFilter::RequestData(vtkInformation *vtkNotUsed(request),
    vtkInformationVector **input, vtkInformationVector *output)
{
  vtkDataSet *dsIn = vtkDataSet::SafeDownCast(
      input[0]->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataSet *dsOut = vtkDataSet::SafeDownCast(
      output->GetInformationObject(0)->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray* array = this->GetInputArrayToProcess(0, input);

  if(dsIn == NULL || array == NULL || dsOut == NULL
     || array->GetNumberOfComponents() != 3 ||
     this->GetInputArrayAssociation(0, input) != vtkDataObject::FIELD_ASSOCIATION_POINTS)
    {
    vtkDebugMacro("vtkScaleVectorFilter no correctly configured");
    return 1;
    }

  dsOut->ShallowCopy(dsIn);

  vtkPointData* inPD = dsIn->GetPointData();
  vtkPointData* outPD = dsOut->GetPointData();

  outPD->RemoveArray(array->GetName());

  vtkDataArray* outArray = array->NewInstance();
  outArray->SetName(array->GetName());

  string name = array->GetName();
  size_t pos1 = name.find_last_of("[");
  if(pos1 != string::npos)
    {
    string name1 = name.substr(0, pos1);
    size_t noblankpos = name1.find_last_not_of(" ");
    name1 = name1.substr(0, noblankpos+1);

    outArray->SetName(name1.c_str());

    for(int arrayId = 0; arrayId < inPD->GetNumberOfArrays(); arrayId++)
      {
      vtkDataArray* thearray = inPD->GetArray(arrayId);
      if(thearray == NULL)
        continue;

      if(strncmp(name1.c_str(), thearray->GetName(), name1.size()) == 0)
        {
        outPD->RemoveArray(thearray->GetName());
        }
      }
    }

  outPD->AddArray(outArray);
  outArray->Delete();

  outArray->SetNumberOfComponents(array->GetNumberOfComponents());
  outArray->SetNumberOfTuples(array->GetNumberOfTuples());

  if(array->HasAComponentName())
    {
    outArray->CopyComponentNames(array);
    }

  if(array->HasInformation())
    {
    outArray->CopyInformation(array->GetInformation());
    }

  for(vtkIdType id=0; id < outArray->GetNumberOfTuples(); id++)
    {
    double tuple[3];
    array->GetTuple(id, tuple);
    tuple[0] *= this->ScaleFactor;
    tuple[1] *= this->ScaleFactor;
    tuple[2] *= this->ScaleFactor;
    outArray->SetTuple(id, tuple);
    }
  // fix the range keys to match the full range, so that the
  // gui automatically sets the range.
  double range[2];
  vtkInformation* info = outArray->GetInformation();

  array->GetRange(range, -1);
  range[0] = 0;
  info->Set(vtkDataArray::L2_NORM_RANGE(), range, 2);

  if(!outArray->GetInformation()->Has(vtkDataArray::PER_COMPONENT()))
    {
    vtkInformationVector* infoVec = vtkInformationVector::New();
    info->Set( vtkDataArray::PER_COMPONENT(), infoVec );
    infoVec->FastDelete();
    }
  vtkInformationVector* per_comp = info->Get(vtkDataArray::PER_COMPONENT());
  per_comp->SetNumberOfInformationObjects(outArray->GetNumberOfComponents());
  for(int comp = 0; comp < outArray->GetNumberOfComponents(); comp++)
    {
    array->GetRange(range, comp);
    range[0] = fabs(range[0]);
    range[1] = fabs(range[1]);
    double r = (range[0] > range[1] ? range[0] : range[1]);
    range[0] = -r;
    range[1] = r;
    per_comp->GetInformationObject(comp)->Set(
        vtkDataArray::COMPONENT_RANGE(), range, 2 );
    }

  outPD->SetActiveVectors(outArray->GetName());

  return 1;
}

void vtkScaleVectorFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ScaleFactor : " << this->ScaleFactor << endl;
}
