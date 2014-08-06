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
// Author : Anthony Geay

#ifndef vtkExtractCellType_h__
#define vtkExtractCellType_h__

#include "vtkDataSetAlgorithm.h"

class vtkMutableDirectedGraph;

class VTK_EXPORT vtkExtractCellType : public vtkDataSetAlgorithm
{
public:
  static vtkExtractCellType* New();
  vtkTypeMacro(vtkExtractCellType, vtkDataSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual int GetNumberOfGeoTypesArrays();
  const char *GetGeoTypesArrayName(int index);
  int GetGeoTypesArrayStatus(const char *name);
  virtual void SetGeoTypesStatus(const char *name, int status);
  void SetInsideOut(int val);
  // Description:
  // Every time the SIL is updated a this will return a different value.
  virtual int GetSILUpdateStamp();

protected:
  vtkExtractCellType();
  ~vtkExtractCellType();

  int RequestInformation(vtkInformation *request,
      vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  int RequestData(vtkInformation *request, vtkInformationVector **inputVector,
      vtkInformationVector *outputVector);
  // Description:
  // This SIL stores the structure of the mesh/groups/cell types
  // that can be selected.
  virtual void SetSIL(vtkMutableDirectedGraph*);
  vtkGetObjectMacro(SIL, vtkMutableDirectedGraph);
protected:
  vtkMutableDirectedGraph *SIL;
  vtkTimeStamp SILTime;
private:
  vtkExtractCellType(const vtkExtractCellType&);
  void operator=(const vtkExtractCellType&); // Not implemented.
 private:
  //BTX
  //ETX
  class vtkExtractCellTypeInternal;
  vtkExtractCellTypeInternal *Internal;
  int InsideOut;
};

#endif
