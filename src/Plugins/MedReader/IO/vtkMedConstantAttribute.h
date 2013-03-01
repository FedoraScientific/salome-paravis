#ifndef __vtkMedConstantAttribute_h_
#define __vtkMedConstantAttribute_h_

#include "vtkObject.h"
#include "vtkMed.h"

class vtkAbstractArray;
class vtkMedStructElement;
class vtkMedProfile;

class VTK_EXPORT vtkMedConstantAttribute : public vtkObject
{
public :
  static vtkMedConstantAttribute* New();
  vtkTypeMacro(vtkMedConstantAttribute, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the name of this attribute
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // The iterator in the med file of this constant attribute
  vtkSetMacro(MedIterator, med_int);
  vtkGetMacro(MedIterator, med_int);

  // Description:
  // The Geometry type this structural elements lies on.
  vtkSetMacro(AttributeType, med_attribute_type);
  vtkGetMacro(AttributeType, med_attribute_type);

  // Description:
  // The dimension of this structural elements model
  vtkSetMacro(NumberOfComponent, med_int);
  vtkGetMacro(NumberOfComponent, med_int);

  // Description:
  // MED_CELL if the support mesh has any cell, MED_NODE otherwise
  vtkSetMacro(SupportEntityType, med_entity_type);
  vtkGetMacro(SupportEntityType, med_entity_type);

  // Description:
  // The name of the profile if any, MED_NO_PROFILE by default
  vtkSetStringMacro(ProfileName);
  vtkGetStringMacro(ProfileName);

  // Description:
  // The Size of the profile if any
  vtkSetMacro(ProfileSize, med_int);
  vtkGetMacro(ProfileSize, med_int);

  // Description:
  // This store the values of this attribute
  virtual void  SetValues(vtkAbstractArray*);
  vtkGetObjectMacro(Values, vtkAbstractArray);

  // Description:
  // This is the pointer to the actual profile used by this attribute
  virtual void  SetProfile(vtkMedProfile*);
  vtkGetObjectMacro(Profile, vtkMedProfile);

  // Description:
  // The Structural element on which lies this attribute
  virtual void  SetParentStructElement(vtkMedStructElement*);
  vtkGetObjectMacro(ParentStructElement, vtkMedStructElement);

protected:
  vtkMedConstantAttribute();
  virtual ~vtkMedConstantAttribute();

  char *Name;
  med_int MedIterator;
  med_attribute_type AttributeType;
  med_int NumberOfComponent;
  med_entity_type SupportEntityType;
  char* ProfileName;
  med_int ProfileSize;
  vtkAbstractArray* Values;
  vtkMedProfile* Profile;

  vtkMedStructElement * ParentStructElement;

private:
  vtkMedConstantAttribute(const vtkMedConstantAttribute&); // Not implemented.
  void operator=(const vtkMedConstantAttribute&); // Not implemented.

};

#endif //__vtkMedConstantAttribute_h_
