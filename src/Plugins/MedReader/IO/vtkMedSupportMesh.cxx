#include "vtkMedSupportMesh.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedFile.h"

#include "vtkStringArray.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

vtkCxxSetObjectMacro(vtkMedSupportMesh, ParentFile, vtkMedFile);

// vtkCxxRevisionMacro(vtkMedSupportMesh, "$Revision$")
vtkStandardNewMacro(vtkMedSupportMesh)

vtkMedSupportMesh::vtkMedSupportMesh()
{
  this->MedIterator = -1;
  this->Name = NULL;
  this->ParentFile = NULL;
  this->AxisName = vtkStringArray::New();
  this->AxisUnit = vtkStringArray::New();
  this->Description = NULL;
  this->AxisType = MED_UNDEF_AXIS_TYPE;
  this->SpaceDimension = 0;
  this->MeshDimension = 0;
}

vtkMedSupportMesh::~vtkMedSupportMesh()
{
  this->SetName(NULL);
  this->SetDescription(NULL);
  this->SetParentFile(NULL);
  this->AxisName->Delete();
  this->AxisUnit->Delete();
}

void vtkMedSupportMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
