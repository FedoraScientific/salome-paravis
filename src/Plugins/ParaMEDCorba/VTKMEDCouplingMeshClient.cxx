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

#include "VTKMEDCouplingMeshClient.hxx"
#include "VTKMEDCouplingUMeshClient.hxx"
#include "VTKMEDCouplingCMeshClient.hxx"

#include "vtkErrorCode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRectilinearGrid.h"

#include <vector>
#include <string>

//vtkErrorMacro("Cannot call Start() twice before calling Finish().");
void ParaMEDMEM2VTK::FillMEDCouplingMeshInstanceFrom(SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr, vtkDataSet *ret)
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_var umeshPtr=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr))
    {
      vtkUnstructuredGrid *ret1=vtkUnstructuredGrid::SafeDownCast(ret);
      if(!ret1)
        {
          vtkErrorWithObjectMacro(ret,"Internal error in ParaMEDCorba plugin : mismatch between VTK type and CORBA type UMesh !");
          return ;
        }
      bool dummy;//VTK bug
      ParaMEDMEM2VTK::FillMEDCouplingUMeshInstanceFrom(umeshPtr,ret1,dummy);//VTK bug
      return ;
    }
  SALOME_MED::MEDCouplingCMeshCorbaInterface_var cmeshPtr=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(cmeshPtr))
    {
      vtkRectilinearGrid *ret1=vtkRectilinearGrid::SafeDownCast(ret);
      if(!ret1)
        {
          vtkErrorWithObjectMacro(ret,"Internal error in ParaMEDCorba plugin : mismatch between VTK type and CORBA type CMesh !");
          return ;
        }
      ParaMEDMEM2VTK::FillMEDCouplingCMeshInstanceFrom(cmeshPtr,ret1);
      return ;
    }
  vtkErrorWithObjectMacro(ret,"Error : CORBA mesh type ! Mesh type not managed !");
}

vtkDataSet *ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance(SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr, bool& isPolyh)
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_var umeshPtr=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr))
    {
      vtkUnstructuredGrid *ret1=vtkUnstructuredGrid::New();
      ParaMEDMEM2VTK::FillMEDCouplingUMeshInstanceFrom(umeshPtr,ret1,isPolyh);
      return ret1;
    }
  SALOME_MED::MEDCouplingCMeshCorbaInterface_var cmeshPtr=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(cmeshPtr))
    {
      vtkRectilinearGrid *ret1=vtkRectilinearGrid::New();
      ParaMEDMEM2VTK::FillMEDCouplingCMeshInstanceFrom(cmeshPtr,ret1);
      return ret1;
    }
  vtkOutputWindowDisplayErrorText("Error : CORBA mesh type ! Mesh type not managed #2 !");
  return 0;
}
