// Copyright (C) 2010-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef __VTKMEDCOUPLINGFIELDCLIENT_HXX__
#define __VTKMEDCOUPLINGFIELDCLIENT_HXX__

#include "ParaMEDMEM2VTK_defines.hxx"
#include "ParaMEDCouplingCorbaServant.hh"

#include <vector>

class vtkDataSet;
class vtkDoubleArray;

namespace ParaMEDMEM2VTK
{
  std::vector<double> FillMEDCouplingFieldDoubleInstanceFrom(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr, vtkDataSet *ret);
  std::vector<double> FillMEDCouplingFieldDoublePartOnly(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr, vtkDataSet *ret);
  ParaMEDMEM2VTK_EXPORT vtkDataSet *BuildFullyFilledFromMEDCouplingFieldDoubleInstance(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr,
                                                                                       std::vector<double>& times);
  ParaMEDMEM2VTK_EXPORT vtkDoubleArray *BuildFromMEDCouplingFieldDoubleArr(SALOME_MED::DataArrayDoubleCorbaInterface_ptr dadPtr);
}

#endif
