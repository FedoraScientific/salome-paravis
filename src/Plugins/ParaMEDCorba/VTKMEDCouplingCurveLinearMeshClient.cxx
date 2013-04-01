// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

#include "VTKMEDCouplingCurveLinearMeshClient.hxx"

#include "vtkPoints.h"
#include "vtkErrorCode.h"
#include "vtkDoubleArray.h"
#include "vtkStructuredGrid.h"

#include <vector>
#include <string>

void ParaMEDMEM2VTK::FillMEDCouplingCurveLinearMeshInstanceFrom(SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr meshPtr, vtkStructuredGrid *ret)
{
  meshPtr->Register();
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyD,tinyI,tinyS);
  int meshStr[3]={1,1,1};
  int meshDim=(*tinyI)[2];
  if(meshDim<0 || meshDim>3)
    vtkErrorWithObjectMacro(ret,"Internal error in ParaMEDCorba plugin : distant curvilinear mesh has a mesh dimension not in [0,3] !");
  for(int i=0;i<meshDim;i++)
    meshStr[i]=(*tinyI)[3+i];
  ret->SetDimensions(meshStr);
  int nbOfNodes=(*tinyI)[3+meshDim];
  int spaceDim=(*tinyI)[3+meshDim+1];
  delete tinyD;
  delete tinyI;
  delete tinyS;
  //
  vtkDoubleArray *da=vtkDoubleArray::New();
  da->SetNumberOfComponents(3);
  da->SetNumberOfTuples(nbOfNodes);
  double *pt=da->GetPointer(0);
  SALOME_TYPES::ListOfDouble *bigD;
  meshPtr->getSerialisationData(tinyI,bigD);
  delete tinyI;
  if(bigD->length()!=nbOfNodes*spaceDim)
    vtkErrorWithObjectMacro(ret,"Internal error in ParaMEDCorba plugin : distant curvilinear mesh, mismatch between informations ! Internal error !");
  for(int i=0;i<nbOfNodes;i++)
    {
      for(int j=0;j<spaceDim;j++,pt++)
        *pt=(*bigD)[spaceDim*i+j];
      for(int j=spaceDim;j<3;j++,pt++)
        *pt=0.;
    }
  delete bigD;
  vtkPoints *points=vtkPoints::New();
  ret->SetPoints(points);
  points->SetData(da);
  points->Delete();
  da->Delete();
  meshPtr->UnRegister();
}
