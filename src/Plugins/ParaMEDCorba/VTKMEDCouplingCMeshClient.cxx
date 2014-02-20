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

#include "VTKMEDCouplingCMeshClient.hxx"

#include "vtkErrorCode.h"
#include "vtkDoubleArray.h"
#include "vtkRectilinearGrid.h"

#include <vector>
#include <string>

void ParaMEDMEM2VTK::FillMEDCouplingCMeshInstanceFrom(SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr meshPtr, vtkRectilinearGrid *ret)
{
  meshPtr->Register();
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyD,tinyI,tinyS);
  int sizePerAxe[3]={1,1,1};
  bool isOK[3]={false,false,false};
  if((*tinyI)[0]>0)
    { sizePerAxe[0]=(*tinyI)[0]; isOK[0]=true; }
  if((*tinyI)[1]>0)
    { sizePerAxe[1]=(*tinyI)[1]; isOK[1]=true; }
  if((*tinyI)[2]>0)
    { sizePerAxe[2]=(*tinyI)[2]; isOK[2]=true; }
  ret->SetDimensions(sizePerAxe[0],sizePerAxe[1],sizePerAxe[2]);
  delete tinyI;
  delete tinyS;
  delete tinyD;
  SALOME_TYPES::ListOfDouble *bigD;
  meshPtr->getSerialisationData(tinyI,bigD);
  delete tinyI;
  int offset=0;
  vtkDoubleArray *da=0;
  if(isOK[0])
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[0]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[0];i++)
        pt[i]=(*bigD)[i];
      ret->SetXCoordinates(da);
      da->Delete();
      offset+=sizePerAxe[0];
    }
  if(isOK[1])
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[1]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[1];i++)
        pt[i]=(*bigD)[offset+i];
      ret->SetYCoordinates(da);
      da->Delete();
      offset+=sizePerAxe[1];
    }
  else
    {
      da=vtkDoubleArray::New(); da->SetNumberOfTuples(1); da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0); *pt=0.; ret->SetYCoordinates(da); da->Delete();
    }
  if(isOK[2])
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[2]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[2];i++)
        pt[i]=(*bigD)[offset+i];
      ret->SetZCoordinates(da);
      da->Delete();
    }
  else
    {
      da=vtkDoubleArray::New(); da->SetNumberOfTuples(1); da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0); *pt=0.; ret->SetZCoordinates(da); da->Delete();
    }
  delete bigD;
  meshPtr->UnRegister();
}
