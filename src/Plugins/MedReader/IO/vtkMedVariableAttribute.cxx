#include "vtkMedVariableAttribute.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedStructElement.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

// vtkCxxRevisionMacro(vtkMedVariableAttribute, "$Revision$")
vtkStandardNewMacro(vtkMedVariableAttribute);

vtkCxxSetObjectMacro(vtkMedVariableAttribute, ParentStructElement, vtkMedStructElement);

vtkMedVariableAttribute::vtkMedVariableAttribute()
{
  this->Name = NULL;
  this->AttributeType = MED_ATT_UNDEF;
  this->NumberOfComponent = 0;
  this->ParentStructElement = NULL;
}

vtkMedVariableAttribute::~vtkMedVariableAttribute()
{
  this->SetName(NULL);
  this->SetParentStructElement(NULL);
}

void  vtkMedVariableAttribute::Load(vtkMedEntityArray* array)
{
  this->GetParentStructElement()->GetParentFile()->GetMedDriver()->LoadVariableAttribute(this, array);
}

void vtkMedVariableAttribute::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
