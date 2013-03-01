#include "vtkMedConstantAttribute.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"

#include "vtkAbstractArray.h"
#include "vtkMedStructElement.h"
#include "vtkMedProfile.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

// vtkCxxRevisionMacro(vtkMedConstantAttribute, "$Revision$")
vtkStandardNewMacro(vtkMedConstantAttribute);

vtkCxxSetObjectMacro(vtkMedConstantAttribute, Values, vtkAbstractArray);
vtkCxxSetObjectMacro(vtkMedConstantAttribute, ParentStructElement, vtkMedStructElement);
vtkCxxSetObjectMacro(vtkMedConstantAttribute, Profile, vtkMedProfile);

vtkMedConstantAttribute::vtkMedConstantAttribute()
{
  this->Name = NULL;
  this->AttributeType = MED_ATT_UNDEF;
  this->NumberOfComponent = 0;
  this->SupportEntityType = MED_UNDEF_ENTITY_TYPE;
  this->ProfileName = NULL;
  this->ProfileSize = 0;
  this->Values = NULL;
  this->ParentStructElement = NULL;
  this->Profile = NULL;
}

vtkMedConstantAttribute::~vtkMedConstantAttribute()
{
  this->SetName(NULL);
  this->SetProfileName(NULL);
  this->SetParentStructElement(NULL);
  this->SetValues(NULL);
  this->SetProfile(NULL);
}

void vtkMedConstantAttribute::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
