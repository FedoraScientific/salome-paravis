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

#ifndef _vtkScaleVectorFilter_h
#define _vtkScaleVectorFilter_h

#include "vtkPassInputTypeAlgorithm.h"

class VTK_EXPORT vtkScaleVectorFilter: public vtkPassInputTypeAlgorithm
{
public:
  static vtkScaleVectorFilter *New();
  vtkTypeMacro(vtkScaleVectorFilter,vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // This is the factor applied to the vector field.
  vtkSetClampMacro(ScaleFactor,double, -1, 1);
  vtkGetMacro(ScaleFactor,double);

protected:
  vtkScaleVectorFilter();
  ~vtkScaleVectorFilter();

  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);

  int FillInputPortInformation(int port,
                               vtkInformation* info);

  double ScaleFactor;

private:
  vtkScaleVectorFilter(const vtkScaleVectorFilter&);
  void operator =(const vtkScaleVectorFilter&);
};
#endif
