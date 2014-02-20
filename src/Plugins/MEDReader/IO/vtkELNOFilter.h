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

#ifndef _vtkELNOFilter_h
#define _vtkELNOFilter_h

#include "vtkQuadraturePointsGenerator.h"

class VTK_EXPORT vtkELNOFilter: public vtkQuadraturePointsGenerator
{
public:
  static vtkELNOFilter *New();
  vtkTypeMacro(vtkELNOFilter,vtkQuadraturePointsGenerator);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // This is the factor applied to shrink the cell before extracting
  // the ELNO points.
  // A value of 0 shrinks the cells to their center, and a value of 1
  // do not shrink the cell at all.
  // default value 0.5
  vtkSetMacro(ShrinkFactor,double);
  vtkGetMacro(ShrinkFactor,double);

protected:
  vtkELNOFilter();
  ~vtkELNOFilter();

  int RequestData(vtkInformation *, vtkInformationVector **,
      vtkInformationVector *);

  double ShrinkFactor;

private:
  vtkELNOFilter(const vtkELNOFilter&);
  void operator =(const vtkELNOFilter&);
};
#endif
