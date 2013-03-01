#ifndef __vtkMedSupportMesh_h_
#define __vtkMedSupportMesh_h_

#include "vtkObject.h"
#include "vtkMed.h"
#include "vtkMedSetGet.h"

class vtkStringArray;
class vtkMedFile;

class VTK_EXPORT vtkMedSupportMesh : public vtkObject
{
public :
  static vtkMedSupportMesh* New();
  vtkTypeMacro(vtkMedSupportMesh, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The iterator to identify this struct element model in the med file
  vtkSetMacro(MedIterator, med_int);
  vtkGetMacro(MedIterator, med_int);

  // Description:
  // This is the name of this structural element model
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // This is the dsecription of this structural element model
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // The dimension of this support mesh lives in
  vtkSetMacro(SpaceDimension, med_int);
  vtkGetMacro(SpaceDimension, med_int);

  // Description:
  // The dimension of this support mesh model
  vtkSetMacro(MeshDimension, med_int);
  vtkGetMacro(MeshDimension, med_int);;

  // Description:
  // The dimension of this support mesh model
  vtkSetMacro(AxisType, med_axis_type);
  vtkGetMacro(AxisType, med_axis_type);;

  // Description:
  // The file this support mesh is stored in.
  virtual void SetParentFile(vtkMedFile*);
  vtkGetObjectMacro(ParentFile, vtkMedFile);

  // Dsecription:
  // This array stores the units of the axis
  vtkGetObjectMacro(AxisName, vtkStringArray);

  // Description:
  // This array store the units of thi support mesh for each axis
  vtkGetObjectMacro(AxisUnit, vtkStringArray);

protected:
  vtkMedSupportMesh();
  virtual ~vtkMedSupportMesh();

  med_int MedIterator;
  med_int SpaceDimension;
  med_int MeshDimension;
  char *Name;
  char *Description;
  med_axis_type AxisType;
  vtkStringArray *AxisName;
  vtkStringArray *AxisUnit;

  vtkMedFile* ParentFile;

private:
  vtkMedSupportMesh(const vtkMedSupportMesh&); // Not implemented.
  void operator=(const vtkMedSupportMesh&); // Not implemented.

};

#endif //__vtkMedSupportMesh_h_
