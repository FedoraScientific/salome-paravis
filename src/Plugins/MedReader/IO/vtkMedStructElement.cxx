#include "vtkMedStructElement.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedVariableAttribute.h"
#include "vtkMedConstantAttribute.h"
#include "vtkMedFile.h"
#include "vtkMedMesh.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

vtkCxxGetObjectVectorMacro(vtkMedStructElement, VariableAttribute, vtkMedVariableAttribute);
vtkCxxSetObjectVectorMacro(vtkMedStructElement, VariableAttribute, vtkMedVariableAttribute);
vtkCxxGetObjectVectorMacro(vtkMedStructElement, ConstantAttribute, vtkMedConstantAttribute);
vtkCxxSetObjectVectorMacro(vtkMedStructElement, ConstantAttribute, vtkMedConstantAttribute);

vtkCxxSetObjectMacro(vtkMedStructElement, ParentFile, vtkMedFile);
vtkCxxSetObjectMacro(vtkMedStructElement, SupportMesh, vtkMedMesh);

// vtkCxxRevisionMacro(vtkMedStructElement, "$Revision$")
vtkStandardNewMacro(vtkMedStructElement)

vtkMedStructElement::vtkMedStructElement()
{
  this->MedIterator = -1;
  this->Name = NULL;
  this->GeometryType = MED_UNDEF_GEOTYPE;
  this->ModelDimension = 0;
  this->SupportMeshName = NULL;
  this->SupportMesh = NULL;
  this->SupportEntityType = MED_UNDEF_ENTITY_TYPE;
  this->SupportNumberOfNode = 0;
  this->SupportNumberOfCell = 0;
  this->SupportGeometryType = MED_UNDEF_GEOTYPE;
  this->AnyProfile = MED_FALSE;
  this->VariableAttribute = new vtkObjectVector<vtkMedVariableAttribute>();
  this->ConstantAttribute = new vtkObjectVector<vtkMedConstantAttribute>();
  this->ParentFile = NULL;
}

vtkMedStructElement::~vtkMedStructElement()
{
  this->SetName(NULL);
  this->SetSupportMeshName(NULL);
  this->SetParentFile(NULL);
  this->SetSupportMesh(NULL);
  delete this->VariableAttribute;
  delete this->ConstantAttribute;
}

void  vtkMedStructElement::LoadVariableAttributes(vtkMedEntityArray* array)
{
  for(int varattit = 0; varattit < this->GetNumberOfVariableAttribute(); varattit++)
    {
    vtkMedVariableAttribute* varatt = this->GetVariableAttribute(varattit);
    varatt->Load(array);
    }
}

int vtkMedStructElement::GetConnectivitySize()
{
  if(strcmp(this->Name, MED_PARTICLE_NAME) == 0
     || this->SupportEntityType != MED_CELL)
    return this->SupportNumberOfNode;

  return this->SupportNumberOfCell *
      vtkMedUtilities::GetNumberOfPoint(this->SupportGeometryType);
}

void vtkMedStructElement::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
