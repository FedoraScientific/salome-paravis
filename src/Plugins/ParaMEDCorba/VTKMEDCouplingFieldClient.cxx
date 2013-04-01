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

#include "VTKMEDCouplingFieldClient.hxx"
#include "VTKMEDCouplingMeshClient.hxx"

#include "vtkErrorCode.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkUnstructuredGrid.h"

#include <string>
#include <sstream>

std::vector<double> ParaMEDMEM2VTK::FillMEDCouplingFieldDoubleInstanceFrom(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr, vtkDataSet *ret)
{
  fieldPtr->Register();
  //
  SALOME_MED::MEDCouplingMeshCorbaInterface_var meshPtr=fieldPtr->getMesh();
  ParaMEDMEM2VTK::FillMEDCouplingMeshInstanceFrom(meshPtr,ret);
  meshPtr->UnRegister();
  //
  std::vector<double> ret2=FillMEDCouplingFieldDoublePartOnly(fieldPtr,ret);
  fieldPtr->UnRegister();
  //
  return ret2;
}

std::vector<double> ParaMEDMEM2VTK::FillMEDCouplingFieldDoublePartOnly(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr, vtkDataSet *ret)
{
  std::vector<double> ret2;
  //
  SALOME_TYPES::ListOfLong *tinyL;
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfString *tinyS;
  fieldPtr->getTinyInfo(tinyL,tinyD,tinyS);
  //
  int typeOfField=(*tinyL)[0];// 0:ON_CELLS, 1:ON_NODES, 2:ON_GAUSS_PT, 3:ON_GAUSS_NE
  int timeDiscr=(*tinyL)[1];
  int nbOfTuples=(*tinyL)[3];
  int nbOfComponents=(*tinyL)[4];
  std::vector<std::string> comps(nbOfComponents);
  for(int i=0;i<nbOfComponents;i++)
    {
      std::string comp((*tinyS)[i]);
      if(!comp.empty())
        comps[i]=comp;
      else
        {
          std::ostringstream oss; oss << "Comp" << i;
          comps[i]=oss.str();
        }
    }
  std::string name;
  if(timeDiscr!=7)
    name=((*tinyS)[nbOfComponents]);
  else
    name=((*tinyS)[2*nbOfComponents]);
  //
  switch(timeDiscr)
    {
    case 4://NO_TIME
      ret2.resize(1);
      ret2[0]=-1;
      break;
    case 5://ONE_TIME
      ret2.resize(1);
      ret2[0]=(*tinyD)[1];
      break;
    case 6://LINEAR_TIME
    case 7://CONST_ON_TIME_INTERVAL
      ret2.resize(1);
      ret2[0]=((*tinyD)[1]+(*tinyD)[2])/2.;
      break;
    default:
      vtkErrorWithObjectMacro(ret,"Unrecognized time discretization of Field ! Not implemented yet !");
    }
  //
  delete tinyL;
  delete tinyD;
  delete tinyS;
  //
  vtkDoubleArray *var0=vtkDoubleArray::New();
  vtkDoubleArray *var1=0;
  double *var0Ptr=0;
  double *var1Ptr=0;
  var0->SetName(name.c_str());
  var0->SetNumberOfComponents(nbOfComponents);
  var0->SetNumberOfTuples(nbOfTuples);
  for(int i=0;i<nbOfComponents;i++)
    var0->SetComponentName(i,comps[i].c_str());
  var0Ptr=var0->GetPointer(0);
  if(timeDiscr==7)
    {
      var1->SetNumberOfComponents(nbOfComponents);
      var1->SetNumberOfTuples(nbOfTuples);
      var1Ptr=var1->GetPointer(0);
      std::ostringstream oss; oss << name << "_end_array";
      var1->SetName(oss.str().c_str());
    }
  //
  SALOME_TYPES::ListOfLong *bigDataI;
  SALOME_TYPES::ListOfDouble2 *bigArr;
  fieldPtr->getSerialisationData(bigDataI,bigArr);
  delete bigDataI;//for the moment gauss points are not dealt
  SALOME_TYPES::ListOfDouble& oneArr=(*bigArr)[0];
  int nbVals=oneArr.length();
  for(int i=0;i<nbVals;i++)
    var0Ptr[i]=oneArr[i];
  if(timeDiscr==7)
    {
      SALOME_TYPES::ListOfDouble& scndArr=(*bigArr)[1];
      for(int i=0;i<nbVals;i++)
        var1Ptr[i]=scndArr[i];
    }
  delete bigArr;
  //
  switch(typeOfField)
    {
    case 0://ON_CELLS
      ret->GetCellData()->AddArray(var0);
      if(timeDiscr==7)
        ret->GetCellData()->AddArray(var1);
      break;
    case 1://ON_NODES
      ret->GetPointData()->AddArray(var0);
      if(timeDiscr==7)
        ret->GetCellData()->AddArray(var1);
      break;
    default:
      vtkErrorWithObjectMacro(ret,"Not implemented yet for gauss fields and gauss on elements fields !");
    }
  var0->Delete();
  if(timeDiscr==7)
    var1->Delete();
  //
  return ret2;
}

vtkDataSet *ParaMEDMEM2VTK::BuildFullyFilledFromMEDCouplingFieldDoubleInstance(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr,
                                                                               std::vector<double>& times)
{
  fieldPtr->Register();
  SALOME_MED::MEDCouplingMeshCorbaInterface_var meshPtr=fieldPtr->getMesh();
  bool dummy;//VTK bug
  vtkDataSet *ret=ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance(meshPtr,dummy);//VTK bug
  meshPtr->UnRegister();
  //
  std::vector<double> ret2=FillMEDCouplingFieldDoublePartOnly(fieldPtr,ret);
  times=ret2;
  //
  fieldPtr->UnRegister();
  return ret;
}

vtkDoubleArray *ParaMEDMEM2VTK::BuildFromMEDCouplingFieldDoubleArr(SALOME_MED::DataArrayDoubleCorbaInterface_ptr dadPtr)
{
  vtkDoubleArray *ret=vtkDoubleArray::New();
  //
  SALOME_TYPES::ListOfLong *tinyL=0;
  SALOME_TYPES::ListOfDouble *bigD=0;
  SALOME_TYPES::ListOfString *tinyS=0;
  //
  dadPtr->getTinyInfo(tinyL,tinyS);
  int nbOfTuples=(*tinyL)[0];
  int nbOfCompo=(*tinyL)[1];
  std::string name((*tinyS)[0]);
  std::vector<std::string> comps(nbOfCompo);
  for(int i=0;i<nbOfCompo;i++)
    comps[i]=(*tinyS)[i+1];
  delete tinyL;
  delete tinyS;
  //
  ret->SetName(name.c_str());
  ret->SetNumberOfComponents(nbOfCompo);
  ret->SetNumberOfTuples(nbOfTuples);
  for(int i=0;i<nbOfCompo;i++)
    {
      if(!comps[i].empty())
        ret->SetComponentName(i,comps[i].c_str());
      else
        {
          std::ostringstream oss; oss << "Comp" << i;
          ret->SetComponentName(i,oss.str().c_str());
        }
    }
  int nbElems=nbOfCompo*nbOfTuples;
  double *pt=ret->GetPointer(0);
  dadPtr->getSerialisationData(bigD);
  for(int i=0;i<nbElems;i++)
    pt[i]=(*bigD)[i];
  delete bigD;
  //
  return ret;
}
