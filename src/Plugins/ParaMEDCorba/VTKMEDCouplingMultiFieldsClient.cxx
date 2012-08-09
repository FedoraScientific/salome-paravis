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

// To access to vtkUnstructuredGrid::Faces and FaceLocations
#define protected public

#include "VTKMEDCouplingMultiFieldsClient.hxx"
#include "VTKMEDCouplingMeshClient.hxx"
#include "VTKMEDCouplingFieldClient.hxx"

#include "vtkUnstructuredGrid.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkErrorCode.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkPointData.h"

#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>

const double ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::EPS_TIME=1e-7;

ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::MEDCouplingMultiFieldsFetcher(int bufferingPolicy,
									     SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr mfieldsPtr):_effective_pol(bufferingPolicy),_mfields_ptr_released(false)
{
  _mfields_ptr=SALOME_MED::MEDCouplingMultiFieldsCorbaInterface::_duplicate(mfieldsPtr);
  _mfields_ptr->Register();
}

ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::~MEDCouplingMultiFieldsFetcher()
{
  for(std::vector<vtkDataSet *>::iterator it=_meshes.begin();it!=_meshes.end();it++)
    {
      if(*it)
	(*it)->Delete();
    }
  for(std::vector<vtkDoubleArray *>::iterator it2=_arrays.begin();it2!=_arrays.end();it2++)
    {
      if(*it2)
	(*it2)->Delete();
    }
  if(!_mfields_ptr_released)
    _mfields_ptr->UnRegister();
}

std::vector<double> ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::getTimeStepsForPV()
{
  retrievesMainTinyInfo();
  int nbOfFields=_mesh_id_per_field.size();
  //
  _time_label_per_field.resize(nbOfFields);
  SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_var fotPtr=SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface::_narrow(_mfields_ptr);
  if(CORBA::is_nil(fotPtr))
    {
      for(CORBA::Long i=0;i<nbOfFields;i++)
	_time_label_per_field[i]=(double)i;
    }
  else
    {
      double tmp=0.;
      for(CORBA::Long i=0;i<nbOfFields;i++)
	{
	  if(!_time_def_per_field[i].empty())
	    _time_label_per_field[i]=_time_def_per_field[i].front();
	  else
	    _time_label_per_field[i]=tmp++;
	}
    }
  return _time_label_per_field;
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::fetchRegardingPolicy()
{
  if(_effective_pol>=10)
    {
      fetchAll();
      return ;
    }
  if(_effective_pol>=1 && _effective_pol<=9)
    {
      fetchMeshes();
      return ;
    }
}

vtkDataSet *ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::buildDataSetOnTime(double time)
{
  int fieldId=getPosGivenTimeLabel(time);
  if(fieldId<0)
    return 0;
  fetchDataIfNeeded(fieldId);
  int meshId=_mesh_id_per_field[fieldId];
  vtkDataSet *ret0=_meshes[meshId];
  std::string clsName=ret0->GetClassName();
  if(clsName=="vtkUnstructuredGrid")
    {
      vtkUnstructuredGrid *ret1=vtkUnstructuredGrid::New();
      ret1->DeepCopy(ret0);
      if(_is_meshes_polyhedron[meshId])//bug VTK polyhedron
        {//bug VTK polyhedron part
          ret1->Faces->UnRegister(ret1);
          ret1->Faces=vtkIdTypeArray::New();
          ret1->Faces->DeepCopy(((vtkUnstructuredGrid *)ret0)->GetFaces());
          ret1->Faces->Register(ret1);
          ret1->Faces->Delete();
          ret1->FaceLocations->UnRegister(ret1);
          ret1->FaceLocations=vtkIdTypeArray::New();
          ret1->FaceLocations->DeepCopy(((vtkUnstructuredGrid *)ret0)->GetFaceLocations());
          ret1->FaceLocations->Register(ret1);
          ret1->FaceLocations->Delete();
        }//end bug VTK polyhedron part
      appendFieldValueOnAlreadyFetchedData(ret1,fieldId);
      applyBufferingPolicy();
      return ret1;
    }
  if(clsName=="vtkRectilinearGrid")
    {
      vtkRectilinearGrid *ret1=vtkRectilinearGrid::New();
      ret1->DeepCopy(ret0);
      appendFieldValueOnAlreadyFetchedData(ret1,fieldId);
      applyBufferingPolicy();
      return ret1;
    }
  return 0;
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::retrievesMainTinyInfo()
{
  SALOME_TYPES::ListOfLong *tinyL=0;
  SALOME_TYPES::ListOfDouble *tinyD=0;
  SALOME_TYPES::ListOfString *tinyS=0;
  //
  CORBA::Long nbOfArrays;
  CORBA::Long nbOfFields;
  CORBA::Long nbOfMeshes=_mfields_ptr->getMainTinyInfo(tinyL,tinyD,nbOfArrays,nbOfFields);
  int sz=(*tinyL)[0];//nbOfFields
  //int sz2=(*tinyL)[1];//sigma(nbOfArraysPerField)
  _time_discr_per_field.resize(sz);//4 : NO_TIME  5:ONE_TIME 6:LINEAR_TIME 7:CONST_ON_TIME_INTERVAL
  _mesh_id_per_field.resize(sz);
  _array_ids_per_field.resize(sz);
  _time_def_per_field.resize(sz);
  int offsetTime=0;
  int offsetArrays=0;
  for(int i=0;i<sz;i++)
    {
      _mesh_id_per_field[i]=(*tinyL)[3+i];
      int nbOfArrayForCurField=(*tinyL)[sz+3+i];
      _array_ids_per_field[i].resize(nbOfArrayForCurField);
      for(int k=0;k<nbOfArrayForCurField;k++)
	_array_ids_per_field[i][k]=(*tinyL)[5*sz+3+offsetArrays+k];
      _time_discr_per_field[i]=(*tinyL)[2*sz+3+i];
      int nbOfTimeSpot=(*tinyL)[3*sz+3+i]-1;//-1 because time precision is not useful here.
      _time_def_per_field[i].resize(nbOfTimeSpot);
      for(int j=0;j<nbOfTimeSpot;j++)
	_time_def_per_field[i][j]=(*tinyD)[offsetTime+1+j];
      offsetTime+=nbOfTimeSpot+1;
      offsetArrays+=nbOfArrayForCurField;
    }
  delete tinyL;
  delete tinyD;
  //
  _meshes.resize(nbOfMeshes+1);
  _is_meshes_polyhedron.resize(nbOfMeshes+1);
  _arrays.resize(nbOfArrays+1);
  //
  _info_per_field.resize(nbOfFields);
  for(int i=0;i<nbOfFields;i++)
    {
      _mfields_ptr->getTinyInfo(i,tinyL,tinyD,tinyS);
      _info_per_field[i]._type=(*tinyL)[0];
      _info_per_field[i]._name=(*tinyS)[0];
      delete tinyL;
      delete tinyD;
      delete tinyS;
    }
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::fetchAll()
{
  fetchMeshes();
  int nbOfArrays=_arrays.size();
  for(int i=0;i<nbOfArrays;i++)
    {
      SALOME_MED::DataArrayDoubleCorbaInterface_var daPtr=_mfields_ptr->getArray(i);
      if(_arrays[i])
	_arrays[i]->Delete();
      _arrays[i]=ParaMEDMEM2VTK::BuildFromMEDCouplingFieldDoubleArr(daPtr);
      daPtr->UnRegister();
    }
  unregisterRemoteServantIfAllFetched();
}

/*!
 * Fetches meshes without regarding if already fetched
 */
void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::fetchMeshes()
{
  int nbOfMeshes=_meshes.size();
  for(int i=0;i<nbOfMeshes;i++)
    {
      SALOME_MED::MEDCouplingMeshCorbaInterface_var mPtr=_mfields_ptr->getMeshWithId(i);
      if(_meshes[i])
	_meshes[i]->Delete();
      bool polyh=false;//bug VTK
      _meshes[i]=ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance(mPtr,polyh);//bug VTK
      _is_meshes_polyhedron[i]=polyh;//bug VTK
      mPtr->UnRegister();
    }
  unregisterRemoteServantIfAllFetched();
}

/*!
 * For a field with id 'fieldId' this method CORBA fetch, if needed, basic data.
 * 'fieldId' should be correct no check of that is done !
 */
void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::fetchDataIfNeeded(int fieldId)
{
  std::vector<int> arrayIds=_array_ids_per_field[fieldId];
  int meshId=_mesh_id_per_field[fieldId];
  if(!_meshes[meshId])
    {
      SALOME_MED::MEDCouplingMeshCorbaInterface_var mPtr=_mfields_ptr->getMeshWithId(meshId);
      bool polyh=false;//bug VTK
      _meshes[meshId]=ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance(mPtr,polyh);//bug VTK
      _is_meshes_polyhedron[meshId]=polyh;//bug VTK
      mPtr->UnRegister();
    }
  for(std::vector<int>::const_iterator it=arrayIds.begin();it!=arrayIds.end();it++)
    {
      if(!_arrays[*it])
        {
          SALOME_MED::DataArrayDoubleCorbaInterface_var daPtr=_mfields_ptr->getArray(*it);
          _arrays[*it]=ParaMEDMEM2VTK::BuildFromMEDCouplingFieldDoubleArr(daPtr);
          daPtr->UnRegister();
        }
    }
  unregisterRemoteServantIfAllFetched();
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::unregisterRemoteServantIfAllFetched()
{
  for(std::vector<vtkDataSet *>::iterator it=_meshes.begin();it!=_meshes.end();it++)
    {
      if((*it)==0)
	return ;
    }
  for(std::vector<vtkDoubleArray *>::iterator it2=_arrays.begin();it2!=_arrays.end();it2++)
    {
      if((*it2)==0)
	return ;
    }
  if(!_mfields_ptr_released)
    {
      _mfields_ptr_released=true;
      _mfields_ptr->UnRegister();
    }
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::applyBufferingPolicy()
{
  if(_effective_pol==0)
    {//
      for(std::vector<vtkDataSet *>::iterator it=_meshes.begin();it!=_meshes.end();it++)
        {
          if(*it)
            {
              (*it)->Delete();
              *it=0;
            }
        }
      for(std::vector<vtkDoubleArray *>::iterator it2=_arrays.begin();it2!=_arrays.end();it2++)
        {
          if(*it2)
            {
              (*it2)->Delete();
              *it2=0;
            }
        }
    }
  //else nothing to do let the plugin bufferize
}

void ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::appendFieldValueOnAlreadyFetchedData(vtkDataSet *ds, int fieldId)
{
  const TinyInfoOnField& info=_info_per_field[fieldId];
  vtkDoubleArray *arr=_arrays[_array_ids_per_field[fieldId].front()];
  arr->SetName(info._name.c_str());
  if(info._type==0)//ON_CELLS
    {
      ds->GetCellData()->AddArray(arr);
      return ;
    }
  if(info._type==1)//ON_NODES
    {
      ds->GetPointData()->AddArray(arr);
      return ;
    }
}

int ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher::getPosGivenTimeLabel(double t)
{
  int nbOfFields=_time_label_per_field.size();
  for(int i=0;i<nbOfFields;i++)
    if(fabs(_time_label_per_field[i]-t)<EPS_TIME)
      return i;
  //2nd chance
  std::vector<double>::iterator it=std::find_if(_time_label_per_field.begin(),_time_label_per_field.end(),
                                                std::bind2nd(std::greater<double>(),t));
  if(it!=_time_label_per_field.end() && it!=_time_label_per_field.end())
    return std::distance(_time_label_per_field.begin(),it);
  //
  std::ostringstream oss;
  oss << "Unexisting time : " << t << " Not in ";
  std::copy(_time_label_per_field.begin(),_time_label_per_field.end(),std::ostream_iterator<double>(oss," "));
  oss << " !";
  vtkOutputWindowDisplayErrorText(oss.str().c_str());
  return -1;
}
