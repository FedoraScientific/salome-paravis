#ifndef __vtkMedVariableAttribute_h_
#define __vtkMedVariableAttribute_h_

#include "vtkObject.h"
#include "vtkMed.h"
#include "vtkMedSetGet.h"

class vtkAbstractArray;
class vtkMedStructElement;
class vtkMedEntityArray;

class VTK_EXPORT vtkMedVariableAttribute : public vtkObject
{
public :
  static vtkMedVariableAttribute* New();
  vtkTypeMacro(vtkMedVariableAttribute, vtkObject)
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
  // The Structural element on which lies this attribute
  virtual void  SetParentStructElement(vtkMedStructElement*);
  vtkGetObjectMacro(ParentStructElement, vtkMedStructElement);

  // Description:
  // Load the values associated with this attribute for the given entity array;
  void  Load(vtkMedEntityArray*);

protected:
  vtkMedVariableAttribute();
  virtual ~vtkMedVariableAttribute();

  char *Name;
  med_int MedIterator;
  med_attribute_type AttributeType;
  med_int NumberOfComponent;

  vtkMedStructElement* ParentStructElement;

private:
  vtkMedVariableAttribute(const vtkMedVariableAttribute&); // Not implemented.
  void operator=(const vtkMedVariableAttribute&); // Not implemented.
};

#endif //__vtkMedVariableAttribute_h_
