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

#include "VTKParaMEDFieldClient.hxx"
#include "VTKMEDCouplingFieldClient.hxx"

#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiBlockDataGroupFilter.h"
#include "vtkCompositeDataToUnstructuredGridFilter.h"

std::vector<double> ParaMEDMEM2VTK::FillMEDCouplingParaFieldDoubleInstanceFrom(SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr, int begin, int end,
                                                                               vtkMultiBlockDataSet *ret)
{
  std::vector<double> ret2;
  int nbOfParts=end-begin;
  Engines::IORTab *allSlices=fieldPtr->tior();
  vtkMultiBlockDataGroupFilter *tmp=vtkMultiBlockDataGroupFilter::New();
  for(int i=0;i<nbOfParts;i++)
    {
      CORBA::Object_ptr obj=(*allSlices)[i+begin];
      SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var fieldCorba=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(obj);
      std::vector<double> times;
      vtkDataSet *part=ParaMEDMEM2VTK::BuildFullyFilledFromMEDCouplingFieldDoubleInstance(fieldCorba,times);
      tmp->AddInputData(part);
      part->Delete();
    }
  tmp->Update();
  vtkCompositeDataToUnstructuredGridFilter *tmp2=vtkCompositeDataToUnstructuredGridFilter::New();
  tmp2->SetInputData(tmp->GetOutput());
  tmp2->Update();
  //
  vtkUnstructuredGrid *ret3=tmp2->GetOutput();
  ret->SetBlock(0,ret3);
  //
  tmp->Delete();
  tmp2->Delete();
  delete allSlices;
  return ret2;
}
