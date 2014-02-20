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

#ifndef __vtkTableTo3D_h
#define __vtkTableTo3D_h

#include "vtkPolyDataAlgorithm.h"

#define TABLETO3D_SURFACE 0
#define TABLETO3D_CONTOUR 1

class VTK_EXPORT vtkTableTo3D : public vtkPolyDataAlgorithm
{
public:
  static vtkTableTo3D* New();
  vtkTypeMacro(vtkTableTo3D, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify value to scale diplacement.
  vtkSetMacro(ScaleFactor, double);
  vtkGetMacro(ScaleFactor, double);
    
  // Description:
  // When set to true, the filter will compute optimus scale factor,
  // the value of ScaleFactor variable will be ignored.
  vtkSetMacro(UseOptimusScale, bool);
  vtkGetMacro(UseOptimusScale, bool);
  vtkBooleanMacro(UseOptimusScale, bool);

  // Description:
  // Specify type of presentation: surface of contour.
  vtkSetClampMacro(PresentationType, int,
                   TABLETO3D_SURFACE, TABLETO3D_CONTOUR);
  vtkGetMacro(PresentationType, int);

  // Description:
  // Specify number of contours for contour presentation type.
  vtkSetMacro(NumberOfContours, int);
  vtkGetMacro(NumberOfContours, int);

protected:
  vtkTableTo3D();
  ~vtkTableTo3D();

  // Description:
  // Overridden to specify that input must be a vtkTable.
  virtual int FillInputPortInformation(int port, vtkInformation* info);

  // Description:
  // Convert input vtkTable to vtkPolyData.
  virtual int RequestData(vtkInformation* request,
    vtkInformationVector** inputVector, vtkInformationVector* outputVector);

  double ScaleFactor;
  bool UseOptimusScale;
  int PresentationType;
  int NumberOfContours;

private:
  vtkTableTo3D(const vtkTableTo3D&); // Not implemented.
  void operator=(const vtkTableTo3D&); // Not implemented.
};

#endif
