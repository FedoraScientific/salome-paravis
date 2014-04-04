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
// Author : Anthony Geay

#include "MEDTimeReq.hxx"
#include "MEDUtilities.hxx"

#include "MEDFileFieldRepresentationTree.hxx"
#include "MEDCouplingFieldDiscretization.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "InterpKernelGaussCoords.hxx"
#include "MEDFileData.hxx"
#include "SauvReader.hxx"

#include "vtkXMLUnstructuredGridWriter.h"//

#include "vtkUnstructuredGrid.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkUnsignedCharArray.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformation.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"

#include "vtksys/stl/string"
#include "vtksys/ios/fstream"
#include "vtksys/stl/algorithm"
#include "vtkMutableDirectedGraph.h"

using namespace ParaMEDMEM;

const char MEDFileFieldRepresentationLeavesArrays::ZE_SEP[]="@@][@@";

const char MEDFileFieldRepresentationLeavesArrays::TS_STR[]="TS";

const char MEDFileFieldRepresentationLeavesArrays::COM_SUP_STR[]="ComSup";

const char MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_CELL_NAME[]="FamilyIdCell";

const char MEDFileFieldRepresentationLeavesArrays::NUM_ID_CELL_NAME[]="NumIdCell";

const char MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME[]="FamilyIdNode";

const char MEDFileFieldRepresentationLeavesArrays::NUM_ID_NODE_NAME[]="NumIdNode";

const char MEDFileFieldRepresentationTree::ROOT_OF_GRPS_IN_TREE[]="zeGrps";

const char MEDFileFieldRepresentationTree::ROOT_OF_FAM_IDS_IN_TREE[]="zeFamIds";

const char MEDFileFieldRepresentationTree::COMPO_STR_TO_LOCATE_MESH_DA[]="-@?|*_";

vtkIdTypeArray *ELGACmp::findOrCreate(const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd, vtkDataSet *ds, bool& isNew) const
{
  vtkIdTypeArray *try0(isExisting(locsReallyUsed,vtkd));
  if(try0)
    {
      isNew=false;
      return try0;
    }
  else
    {
      isNew=true;
      return createNew(globs,locsReallyUsed,vtkd,ds);
    }
}

vtkIdTypeArray *ELGACmp::isExisting(const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd) const
{
  std::vector< std::vector<std::string> >::iterator it(std::find(_loc_names.begin(),_loc_names.end(),locsReallyUsed));
  if(it==_loc_names.end())
    return 0;
  std::size_t pos(std::distance(_loc_names.begin(),it));
  vtkIdTypeArray *ret(_elgas[pos]);
  vtkInformationQuadratureSchemeDefinitionVectorKey *key(vtkQuadratureSchemeDefinition::DICTIONARY());
  for(std::vector<std::pair< vtkQuadratureSchemeDefinition *, unsigned char > >::const_iterator it=_defs[pos].begin();it!=_defs[pos].end();it++)
    {
      key->Set(vtkd->GetInformation(),(*it).first,(*it).second);
    }
  vtkd->GetInformation()->Set(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME(),ret->GetName());
  return ret;
}

vtkIdTypeArray *ELGACmp::createNew(const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd, vtkDataSet *ds) const
{
  static const int VTK_DATA_ARRAY_DELETE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_DELETE;
  std::vector< std::vector<std::string> > locNames(_loc_names);
  std::vector<vtkIdTypeArray *> elgas(_elgas);
  std::vector< std::pair< vtkQuadratureSchemeDefinition *, unsigned char > > defs;
  //
  std::vector< std::vector<std::string> >::const_iterator it(std::find(locNames.begin(),locNames.end(),locsReallyUsed));
  if(it!=locNames.end())
    throw INTERP_KERNEL::Exception("ELGACmp::createNew : Method is expected to be called after isExisting call ! Entry already exists !");
  locNames.push_back(locsReallyUsed);
  vtkIdTypeArray *elga(vtkIdTypeArray::New());
  elga->SetNumberOfComponents(1);
  vtkInformationQuadratureSchemeDefinitionVectorKey *key(vtkQuadratureSchemeDefinition::DICTIONARY());
  std::map<unsigned char,int> m;
  for(std::vector<std::string>::const_iterator it=locsReallyUsed.begin();it!=locsReallyUsed.end();it++)
    {
      vtkQuadratureSchemeDefinition *def(vtkQuadratureSchemeDefinition::New());
      const MEDFileFieldLoc& loc(globs->getLocalization((*it).c_str()));
      INTERP_KERNEL::NormalizedCellType ct(loc.getGeoType());
      const INTERP_KERNEL::CellModel& cm(INTERP_KERNEL::CellModel::GetCellModel(ct));
      int nbGaussPt(loc.getNbOfGaussPtPerCell()),nbPtsPerCell((int)cm.getNumberOfNodes()),dimLoc(loc.getDimension());
      // WARNING : these 2 lines are a workaround, due to users that write a ref element with dimension not equal to dimension of the geometric element.
      std::vector<double> gsCoods2(INTERP_KERNEL::GaussInfo::NormalizeCoordinatesIfNecessary(ct,dimLoc,loc.getGaussCoords()));
      std::vector<double> refCoods2(INTERP_KERNEL::GaussInfo::NormalizeCoordinatesIfNecessary(ct,dimLoc,loc.getRefCoords()));
      double *shape(new double[nbPtsPerCell*nbGaussPt]);
      INTERP_KERNEL::GaussInfo calculator(ct,gsCoods2,nbGaussPt,refCoods2,nbPtsPerCell);
      calculator.initLocalInfo();
      const std::vector<double>& wgths(loc.getGaussWeights());
      for(int i=0;i<nbGaussPt;i++)
        {
          const double *pt0(calculator.getFunctionValues(i));
          if(ct!=INTERP_KERNEL::NORM_HEXA27)
            std::copy(pt0,pt0+nbPtsPerCell,shape+nbPtsPerCell*i);
          else
            {
              for(int j=0;j<27;j++)
                shape[nbPtsPerCell*i+j]=pt0[MEDMeshMultiLev::HEXA27_PERM_ARRAY[j]];
            }
        }
      unsigned char vtkType(MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE[ct]);
      m[vtkType]=nbGaussPt;
      def->Initialize(vtkType,nbPtsPerCell,nbGaussPt,shape,const_cast<double *>(&wgths[0]));
      delete [] shape;
      key->Set(elga->GetInformation(),def,vtkType);
      key->Set(vtkd->GetInformation(),def,vtkType);
      defs.push_back(std::pair< vtkQuadratureSchemeDefinition *, unsigned char >(def,vtkType));
    }
  //
  vtkIdType ncell(ds->GetNumberOfCells());
  int *pt(new int[ncell]),offset(0);
  for(vtkIdType cellId=0;cellId<ncell;cellId++)
    {
      vtkCell *cell(ds->GetCell(cellId));
      int delta(m[cell->GetCellType()]);
      pt[cellId]=offset;
      offset+=delta;
    }
  elga->GetInformation()->Set(MEDUtilities::ELGA(),1);
  elga->SetArray(pt,ncell,0,VTK_DATA_ARRAY_DELETE);
  std::ostringstream oss; oss << "ELGA" << "@" << _loc_names.size();
  std::string ossStr(oss.str());
  elga->SetName(ossStr.c_str());
  elga->GetInformation()->Set(vtkAbstractArray::GUI_HIDE(),1);
  vtkd->GetInformation()->Set(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME(),elga->GetName());
  elgas.push_back(elga);
  //
  _loc_names=locNames;
  _elgas=elgas;
  _defs.push_back(defs);
}

void ELGACmp::appendELGAIfAny(vtkDataSet *ds) const
{
  for(std::vector<vtkIdTypeArray *>::const_iterator it=_elgas.begin();it!=_elgas.end();it++)
    ds->GetCellData()->AddArray(*it);
}

ELGACmp::~ELGACmp()
{
  for(std::vector<vtkIdTypeArray *>::const_iterator it=_elgas.begin();it!=_elgas.end();it++)
    (*it)->Delete();
  for(std::vector< std::vector< std::pair< vtkQuadratureSchemeDefinition *, unsigned char > > >::const_iterator it0=_defs.begin();it0!=_defs.end();it0++)
    for(std::vector< std::pair< vtkQuadratureSchemeDefinition *, unsigned char > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      (*it1).first->Delete();
}

//=

MEDFileFieldRepresentationLeavesArrays::MEDFileFieldRepresentationLeavesArrays():_id(-1)
{
}

MEDFileFieldRepresentationLeavesArrays::MEDFileFieldRepresentationLeavesArrays(const ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS>& arr):ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS>(arr),_activated(false),_id(-1)
{
  std::vector< std::vector<ParaMEDMEM::TypeOfField> > typs((operator->())->getTypesOfFieldAvailable());
  if(typs.size()<1)
    throw INTERP_KERNEL::Exception("There is a big internal problem in MEDLoader ! The field time spitting has failed ! A CRASH will occur soon !");
  if(typs[0].size()!=1)
    throw INTERP_KERNEL::Exception("There is a big internal problem in MEDLoader ! The field spitting by spatial discretization has failed ! A CRASH will occur soon !");
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDiscretization> fd(MEDCouplingFieldDiscretization::New(typs[0][0]));
  std::ostringstream oss2; oss2 << (operator->())->getName() << ZE_SEP << fd->getRepr();
  _ze_name=oss2.str();
}

MEDFileFieldRepresentationLeavesArrays& MEDFileFieldRepresentationLeavesArrays::operator=(const MEDFileFieldRepresentationLeavesArrays& other)
{
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS>::operator=(other);
  _id=-1;
  _activated=false;
  _ze_name=other._ze_name;
  _ze_full_name.clear();
  return *this;
}

void MEDFileFieldRepresentationLeavesArrays::setId(int& id) const
{
  _id=id++;
}

int MEDFileFieldRepresentationLeavesArrays::getId() const
{
  return _id;
}

std::string MEDFileFieldRepresentationLeavesArrays::getZeName() const
{
  return _ze_full_name;
}

void MEDFileFieldRepresentationLeavesArrays::feedSIL(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, const std::string& tsName, const std::string& meshName, const std::string& comSupStr, std::vector<std::string>& names) const
{
  vtkIdType refId(sil->AddChild(root,edge));
  names.push_back(_ze_name);
  std::ostringstream oss3; oss3 << tsName << "/" << meshName << "/" << comSupStr << "/" << _ze_name;
  _ze_full_name=oss3.str();
  //
  if(MEDFileFieldRepresentationTree::IsFieldMeshRegardingInfo(((operator->())->getInfo())))
    {
      sil->AddChild(refId,edge);
      names.push_back(std::string());
    }
}

bool MEDFileFieldRepresentationLeavesArrays::getStatus() const
{
  return _activated;
}

bool MEDFileFieldRepresentationLeavesArrays::setStatus(bool status) const
{
  bool ret(_activated!=status);
  _activated=status;
  return ret;
}

void MEDFileFieldRepresentationLeavesArrays::appendFields(const MEDTimeReq *tr, const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDMeshMultiLev *mml, const ParaMEDMEM::MEDFileMeshStruct *mst, vtkDataSet *ds) const
{
  static const int VTK_DATA_ARRAY_FREE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  static const int VTK_DATA_ARRAY_DELETE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_DELETE;
  tr->setNumberOfTS((operator->())->getNumberOfTS());
  tr->initIterator();
  for(int timeStepId=0;timeStepId<tr->size();timeStepId++,++(*tr))
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeField1TS> f1ts((operator->())->getTimeStepAtPos(tr->getCurrent()));
      MEDFileAnyTypeField1TS *f1tsPtr(f1ts);
      MEDFileField1TS *f1tsPtrDbl(dynamic_cast<MEDFileField1TS *>(f1tsPtr));
      MEDFileIntField1TS *f1tsPtrInt(dynamic_cast<MEDFileIntField1TS *>(f1tsPtr));
      DataArray *crudeArr(0),*postProcessedArr(0);
      if(f1tsPtrDbl)
        crudeArr=f1tsPtrDbl->getUndergroundDataArray();
      else if(f1tsPtrInt)
        crudeArr=f1tsPtrInt->getUndergroundDataArray();
      else
        throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeavesArrays::appendFields : only FLOAT64 and INT32 fields are dealt for the moment !");
      MEDFileField1TSStructItem fsst(MEDFileField1TSStructItem::BuildItemFrom(f1ts,mst));
      f1ts->loadArraysIfNecessary();
      MEDCouplingAutoRefCountObjectPtr<DataArray> v(mml->buildDataArray(fsst,globs,crudeArr));
      postProcessedArr=v;
      //
      std::vector<TypeOfField> discs(f1ts->getTypesOfFieldAvailable());
      if(discs.size()!=1)
        throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeavesArrays::appendFields : internal error ! Number of spatial discretizations must be equal to one !");
      vtkFieldData *att(0);
      switch(discs[0])
        {
        case ON_CELLS:
          {
            att=ds->GetCellData();
            break;
          }
        case ON_NODES:
          {
            att=ds->GetPointData();
            break;
          }
        case ON_GAUSS_NE:
          {
            att=ds->GetFieldData();
            break;
          }
        case ON_GAUSS_PT:
          {
            att=ds->GetFieldData();
            break;
          }
        default:
          throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeavesArrays::appendFields : only CELL and NODE, GAUSS_NE and GAUSS fields are available for the moment !");
        }
      if(f1tsPtrDbl)
        {
          DataArray *vPtr(v); DataArrayDouble *vd(static_cast<DataArrayDouble *>(vPtr));
          vtkDoubleArray *vtkd(vtkDoubleArray::New());
          vtkd->SetNumberOfComponents(vd->getNumberOfComponents());
          for(int i=0;i<vd->getNumberOfComponents();i++)
            vtkd->SetComponentName(i,vd->getInfoOnComponent(i).c_str());
          if(postProcessedArr!=crudeArr)
            {
              vtkd->SetArray(vd->getPointer(),vd->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); vd->accessToMemArray().setSpecificDeallocator(0);
            }
          else
            {
              vtkd->SetArray(vd->getPointer(),vd->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
            }
          std::string name(tr->buildName(f1ts->getName()));
          vtkd->SetName(name.c_str());
          att->AddArray(vtkd);
          vtkd->Delete();
          if(discs[0]==ON_GAUSS_PT)
            {
              bool tmp;
              _elga_cmp.findOrCreate(globs,f1ts->getLocsReallyUsed(),vtkd,ds,tmp);
            }
          if(discs[0]==ON_GAUSS_NE)
            {
              vtkIdTypeArray *elno(vtkIdTypeArray::New());
              elno->SetNumberOfComponents(1);
              vtkIdType ncell(ds->GetNumberOfCells());
              int *pt(new int[ncell]),offset(0);
              std::set<int> cellTypes;
              for(vtkIdType cellId=0;cellId<ncell;cellId++)
                {
                  vtkCell *cell(ds->GetCell(cellId));
                  int delta(cell->GetNumberOfPoints());
                  cellTypes.insert(cell->GetCellType());
                  pt[cellId]=offset;
                  offset+=delta;
                }
              elno->GetInformation()->Set(MEDUtilities::ELNO(),1);
              elno->SetArray(pt,ncell,0,VTK_DATA_ARRAY_DELETE);
              std::string nameElno("ELNO"); nameElno+="@"; nameElno+=name;
              elno->SetName(nameElno.c_str());
              ds->GetCellData()->AddArray(elno);
              vtkd->GetInformation()->Set(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME(),elno->GetName());
              elno->GetInformation()->Set(vtkAbstractArray::GUI_HIDE(),1);
              //
              vtkInformationQuadratureSchemeDefinitionVectorKey *key(vtkQuadratureSchemeDefinition::DICTIONARY());
              for(std::set<int>::const_iterator it=cellTypes.begin();it!=cellTypes.end();it++)
                {
                  const unsigned char *pos(std::find(MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE,MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE+MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE_LGTH,*it));
                  if(pos==MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE+MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE_LGTH)
                    continue;
                  INTERP_KERNEL::NormalizedCellType ct((INTERP_KERNEL::NormalizedCellType)std::distance(MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE,pos));
                  const INTERP_KERNEL::CellModel& cm(INTERP_KERNEL::CellModel::GetCellModel(ct));
                  int nbGaussPt(cm.getNumberOfNodes()),dim(cm.getDimension());
                  vtkQuadratureSchemeDefinition *def(vtkQuadratureSchemeDefinition::New());
                  double *shape(new double[nbGaussPt*nbGaussPt]);
                  std::size_t dummy;
                  const double *gsCoords(MEDCouplingFieldDiscretizationGaussNE::GetLocsFromGeometricType(ct,dummy));
                  const double *refCoords(MEDCouplingFieldDiscretizationGaussNE::GetRefCoordsFromGeometricType(ct,dummy));
                  const double *weights(MEDCouplingFieldDiscretizationGaussNE::GetWeightArrayFromGeometricType(ct,dummy));
                  std::vector<double> gsCoords2(gsCoords,gsCoords+nbGaussPt*dim),refCoords2(refCoords,refCoords+nbGaussPt*dim);
                  INTERP_KERNEL::GaussInfo calculator(ct,gsCoords2,nbGaussPt,refCoords2,nbGaussPt);
                  calculator.initLocalInfo();
                  for(int i=0;i<nbGaussPt;i++)
                    {
                      const double *pt0(calculator.getFunctionValues(i));
                      std::copy(pt0,pt0+nbGaussPt,shape+nbGaussPt*i);
                    }
                  def->Initialize(*it,nbGaussPt,nbGaussPt,shape,const_cast<double *>(weights));
                  delete [] shape;
                  key->Set(elno->GetInformation(),def,*it);
                  key->Set(vtkd->GetInformation(),def,*it);
                  def->Delete();
                }
              //
              elno->Delete();
            }
        }
      else if(f1tsPtrInt)
        {
          DataArray *vPtr(v); DataArrayInt *vi(static_cast<DataArrayInt *>(vPtr));
          vtkIntArray *vtkd(vtkIntArray::New());
          vtkd->SetNumberOfComponents(vi->getNumberOfComponents());
          for(int i=0;i<vi->getNumberOfComponents();i++)
            vtkd->SetComponentName(i,vi->getVarOnComponent(i).c_str());
          if(postProcessedArr!=crudeArr)
            {
              vtkd->SetArray(vi->getPointer(),vi->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); vi->accessToMemArray().setSpecificDeallocator(0);
            }
          else
            {
              vtkd->SetArray(vi->getPointer(),vi->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
            }
          std::string name(tr->buildName(f1ts->getName()));
          vtkd->SetName(name.c_str());
          att->AddArray(vtkd);
          vtkd->Delete();
        }
      else
        throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeavesArrays::appendFields : only FLOAT64 and INT32 fields are dealt for the moment ! Internal Error !");
    }
}

void MEDFileFieldRepresentationLeavesArrays::appendELGAIfAny(vtkDataSet *ds) const
{
  _elga_cmp.appendELGAIfAny(ds);
}

////////////////////

MEDFileFieldRepresentationLeaves::MEDFileFieldRepresentationLeaves():_cached_ds(0)
{
}

MEDFileFieldRepresentationLeaves::MEDFileFieldRepresentationLeaves(const std::vector< ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS> >& arr,
                                                                   const ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFastCellSupportComparator>& fsp):_arrays(arr.size()),_fsp(fsp),_cached_ds(0)
{
  for(std::size_t i=0;i<arr.size();i++)
    _arrays[i]=MEDFileFieldRepresentationLeavesArrays(arr[i]);
}

MEDFileFieldRepresentationLeaves::~MEDFileFieldRepresentationLeaves()
{
  if(_cached_ds)
    _cached_ds->Delete();
}

bool MEDFileFieldRepresentationLeaves::empty() const
{
  const MEDFileFastCellSupportComparator *fcscp(_fsp);
  return fcscp==0 || _arrays.empty();
}

void MEDFileFieldRepresentationLeaves::setId(int& id) const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    (*it).setId(id);
}

std::string MEDFileFieldRepresentationLeaves::getMeshName() const
{
  return _arrays[0]->getMeshName();
}

int MEDFileFieldRepresentationLeaves::getNumberOfArrays() const
{
  return (int)_arrays.size();
}

int MEDFileFieldRepresentationLeaves::getNumberOfTS() const
{
  return _arrays[0]->getNumberOfTS();
}

/*!
 * \param [in] ms is the meshes pointer. It can be used only for information of geometric types. No special processing will be requested on ms.
 */
void MEDFileFieldRepresentationLeaves::feedSIL(const ParaMEDMEM::MEDFileMeshes *ms, vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, const std::string& tsName, const std::string& meshName, const std::string& comSupStr, std::vector<std::string>& names) const
{
  vtkIdType root2(sil->AddChild(root,edge));
  names.push_back(std::string("Arrs"));
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    (*it).feedSIL(sil,root2,edge,tsName,meshName,comSupStr,names);
  //
  vtkIdType root3(sil->AddChild(root,edge));
  names.push_back(std::string("InfoOnGeoType"));
  const ParaMEDMEM::MEDFileMesh *m(0);
  if(ms)
    m=ms->getMeshWithName(meshName);
  const ParaMEDMEM::MEDFileFastCellSupportComparator *fsp(_fsp);
  if(!fsp || fsp->getNumberOfTS()==0)
    return ;
  std::vector< INTERP_KERNEL::NormalizedCellType > gts(fsp->getGeoTypesAt(0,m));
  for(std::vector< INTERP_KERNEL::NormalizedCellType >::const_iterator it2=gts.begin();it2!=gts.end();it2++)
    {
      const INTERP_KERNEL::CellModel& cm(INTERP_KERNEL::CellModel::GetCellModel(*it2));
      std::string cmStr(cm.getRepr()); cmStr=cmStr.substr(5);//skip "NORM_"
      sil->AddChild(root3,edge);
      names.push_back(cmStr);
    }
}

bool MEDFileFieldRepresentationLeaves::containId(int id) const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    if((*it).getId()==id)
      return true;
  return false;
}

bool MEDFileFieldRepresentationLeaves::containZeName(const char *name, int& id) const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    if((*it).getZeName()==name)
      {
        id=(*it).getId();
        return true;
      }
  return false;
}

bool MEDFileFieldRepresentationLeaves::isActivated() const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    if((*it).getStatus())
      return true;
  return false;
}

void MEDFileFieldRepresentationLeaves::printMySelf(std::ostream& os) const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it0=_arrays.begin();it0!=_arrays.end();it0++)
    {
      os << "         - " << (*it0).getZeName() << " (";
      if((*it0).getStatus())
        os << "X";
      else
        os << " ";
      os << ")" << std::endl;
    }
}

void MEDFileFieldRepresentationLeaves::activateAllArrays() const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    (*it).setStatus(true);
}

const MEDFileFieldRepresentationLeavesArrays& MEDFileFieldRepresentationLeaves::getLeafArr(int id) const
{
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    if((*it).getId()==id)
      return *it;
  throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeaves::getLeafArr ! No such id !");
}

std::vector<double> MEDFileFieldRepresentationLeaves::getTimeSteps(const TimeKeeper& tk) const
{
  if(_arrays.size()<1)
    throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeaves::getTimeSteps : the array size must be at least of size one !");
  std::vector<double> ret;
  std::vector< std::pair<int,int> > dtits(_arrays[0]->getTimeSteps(ret));
  return tk.getTimeStepsRegardingPolicy(dtits,ret);
}

std::vector< std::pair<int,int> > MEDFileFieldRepresentationLeaves::getTimeStepsInCoarseMEDFileFormat(std::vector<double>& ts) const
{
  if(!_arrays.empty())
    return _arrays[0]->getTimeSteps(ts);
  else
    {
      ts.clear();
      return std::vector< std::pair<int,int> >();
    }
}

std::string MEDFileFieldRepresentationLeaves::getHumanReadableOverviewOfTS() const
{
  std::ostringstream oss;
  oss << _arrays[0]->getNumberOfTS() << " time steps [" << _arrays[0]->getDtUnit() << "]\n(";
  std::vector<double> ret1;
  std::vector< std::pair<int,int> > ret2(getTimeStepsInCoarseMEDFileFormat(ret1));
  std::size_t sz(ret1.size());
  for(std::size_t i=0;i<sz;i++)
    {
      oss << ret1[i] << " (" << ret2[i].first << "," << ret2[i].second << ")";
      if(i!=sz-1)
        oss << ", ";
      std::string tmp(oss.str());
      if(tmp.size()>200 && i!=sz-1)
        {
          oss << "...";
          break;
        }
    }
  oss << ")";
  return oss.str();
}

void MEDFileFieldRepresentationLeaves::appendFields(const MEDTimeReq *tr, const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDMeshMultiLev *mml, const ParaMEDMEM::MEDFileMeshes *meshes, vtkDataSet *ds) const
{
  if(_arrays.size()<1)
    throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeaves::appendFields : internal error !");
  MEDCouplingAutoRefCountObjectPtr<MEDFileMeshStruct> mst(MEDFileMeshStruct::New(meshes->getMeshWithName(_arrays[0]->getMeshName().c_str())));
  for(std::vector<MEDFileFieldRepresentationLeavesArrays>::const_iterator it=_arrays.begin();it!=_arrays.end();it++)
    if((*it).getStatus())
      {
        (*it).appendFields(tr,globs,mml,mst,ds);
        (*it).appendELGAIfAny(ds);
      }
}

vtkUnstructuredGrid *MEDFileFieldRepresentationLeaves::buildVTKInstanceNoTimeInterpolationUnstructured(MEDUMeshMultiLev *mm) const
{
  static const int VTK_DATA_ARRAY_FREE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  DataArrayDouble *coordsMC(0);
  DataArrayByte *typesMC(0);
  DataArrayInt *cellLocationsMC(0),*cellsMC(0),*faceLocationsMC(0),*facesMC(0);
  bool statusOfCoords(mm->buildVTUArrays(coordsMC,typesMC,cellLocationsMC,cellsMC,faceLocationsMC,facesMC));
  MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> coordsSafe(coordsMC);
  MEDCouplingAutoRefCountObjectPtr<DataArrayByte> typesSafe(typesMC);
  MEDCouplingAutoRefCountObjectPtr<DataArrayInt> cellLocationsSafe(cellLocationsMC),cellsSafe(cellsMC),faceLocationsSafe(faceLocationsMC),facesSafe(facesMC);
  //
  int nbOfCells(typesSafe->getNbOfElems());
  vtkUnstructuredGrid *ret(vtkUnstructuredGrid::New());
  vtkUnsignedCharArray *cellTypes(vtkUnsignedCharArray::New());
  cellTypes->SetArray(reinterpret_cast<unsigned char *>(typesSafe->getPointer()),nbOfCells,0,VTK_DATA_ARRAY_FREE); typesSafe->accessToMemArray().setSpecificDeallocator(0);
  vtkIdTypeArray *cellLocations(vtkIdTypeArray::New());
  cellLocations->SetArray(cellLocationsSafe->getPointer(),nbOfCells,0,VTK_DATA_ARRAY_FREE); cellLocationsSafe->accessToMemArray().setSpecificDeallocator(0);
  vtkCellArray *cells(vtkCellArray::New());
  vtkIdTypeArray *cells2(vtkIdTypeArray::New());
  cells2->SetArray(cellsSafe->getPointer(),cellsSafe->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); cellsSafe->accessToMemArray().setSpecificDeallocator(0);
  cells->SetCells(nbOfCells,cells2);
  cells2->Delete();
  if(faceLocationsMC!=0 && facesMC!=0)
    {
      vtkIdTypeArray *faces(vtkIdTypeArray::New());
      faces->SetArray(facesSafe->getPointer(),facesSafe->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); facesSafe->accessToMemArray().setSpecificDeallocator(0);
      vtkIdTypeArray *faceLocations(vtkIdTypeArray::New());
      faceLocations->SetArray(faceLocationsSafe->getPointer(),faceLocationsSafe->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); faceLocationsSafe->accessToMemArray().setSpecificDeallocator(0);
      ret->SetCells(cellTypes,cellLocations,cells,faceLocations,faces);
      faceLocations->Delete();
      faces->Delete();
    }
  else
    ret->SetCells(cellTypes,cellLocations,cells);
  cellTypes->Delete();
  cellLocations->Delete();
  cells->Delete();
  vtkPoints *pts(vtkPoints::New());
  vtkDoubleArray *pts2(vtkDoubleArray::New());
  pts2->SetNumberOfComponents(3);
  if(!statusOfCoords)
    {
      pts2->SetArray(coordsSafe->getPointer(),coordsSafe->getNbOfElems(),0,VTK_DATA_ARRAY_FREE);
      coordsSafe->accessToMemArray().setSpecificDeallocator(0);
    }
  else
    pts2->SetArray(coordsSafe->getPointer(),coordsSafe->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
  pts->SetData(pts2);
  pts2->Delete();
  ret->SetPoints(pts);
  pts->Delete();
  //
  return ret;
}

vtkRectilinearGrid *MEDFileFieldRepresentationLeaves::buildVTKInstanceNoTimeInterpolationCartesian(ParaMEDMEM::MEDCMeshMultiLev *mm) const
{
  static const int VTK_DATA_ARRAY_FREE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  bool isInternal;
  std::vector< DataArrayDouble * > arrs(mm->buildVTUArrays(isInternal));
  vtkDoubleArray *vtkTmp(0);
  vtkRectilinearGrid *ret(vtkRectilinearGrid::New());
  std::size_t dim(arrs.size());
  if(dim<1 || dim>3)
    throw INTERP_KERNEL::Exception("buildVTKInstanceNoTimeInterpolationCartesian : dimension must be in [1,3] !");
  int sizePerAxe[3]={1,1,1};
  sizePerAxe[0]=arrs[0]->getNbOfElems();
  if(dim>=2)
    sizePerAxe[1]=arrs[1]->getNbOfElems();
  if(dim==3)
    sizePerAxe[2]=arrs[2]->getNbOfElems();
  ret->SetDimensions(sizePerAxe[0],sizePerAxe[1],sizePerAxe[2]);
  vtkTmp=vtkDoubleArray::New();
  vtkTmp->SetNumberOfComponents(1);
  if(isInternal)
    vtkTmp->SetArray(arrs[0]->getPointer(),arrs[0]->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
  else
    { vtkTmp->SetArray(arrs[0]->getPointer(),arrs[0]->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); arrs[0]->accessToMemArray().setSpecificDeallocator(0); }
  ret->SetXCoordinates(vtkTmp);
  vtkTmp->Delete();
  arrs[0]->decrRef();
  if(dim>=2)
    {
      vtkTmp=vtkDoubleArray::New();
      vtkTmp->SetNumberOfComponents(1);
      if(isInternal)
        vtkTmp->SetArray(arrs[1]->getPointer(),arrs[1]->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTmp->SetArray(arrs[1]->getPointer(),arrs[1]->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); arrs[1]->accessToMemArray().setSpecificDeallocator(0); }
      ret->SetYCoordinates(vtkTmp);
      vtkTmp->Delete();
      arrs[1]->decrRef();
    }
  if(dim==3)
    {
      vtkTmp=vtkDoubleArray::New();
      vtkTmp->SetNumberOfComponents(1);
      if(isInternal)
        vtkTmp->SetArray(arrs[2]->getPointer(),arrs[2]->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTmp->SetArray(arrs[2]->getPointer(),arrs[2]->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); arrs[2]->accessToMemArray().setSpecificDeallocator(0); }
      ret->SetZCoordinates(vtkTmp);
      vtkTmp->Delete();
      arrs[2]->decrRef();
    }
  return ret;
}

vtkStructuredGrid *MEDFileFieldRepresentationLeaves::buildVTKInstanceNoTimeInterpolationCurveLinear(ParaMEDMEM::MEDCurveLinearMeshMultiLev *mm) const
{
  static const int VTK_DATA_ARRAY_FREE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  int meshStr[3]={1,1,1};
  DataArrayDouble *coords(0);
  std::vector<int> nodeStrct;
  bool isInternal;
  mm->buildVTUArrays(coords,nodeStrct,isInternal);
  std::size_t dim(nodeStrct.size());
  if(dim<1 || dim>3)
    throw INTERP_KERNEL::Exception("buildVTKInstanceNoTimeInterpolationCurveLinear : dimension must be in [1,3] !");
  meshStr[0]=nodeStrct[0];
  if(dim>=2)
    meshStr[1]=nodeStrct[1];
  if(dim==3)
    meshStr[2]=nodeStrct[2];
  vtkStructuredGrid *ret(vtkStructuredGrid::New());
  ret->SetDimensions(meshStr[0],meshStr[1],meshStr[2]);
  vtkDoubleArray *da(vtkDoubleArray::New());
  da->SetNumberOfComponents(3);
  if(coords->getNumberOfComponents()==3)
    {
      if(isInternal)
        da->SetArray(coords->getPointer(),coords->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);//VTK has not the ownership of double * because MEDLoader main struct has it !
      else
        { da->SetArray(coords->getPointer(),coords->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); coords->accessToMemArray().setSpecificDeallocator(0); }
    }
  else
    {
      MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> coords2(coords->changeNbOfComponents(3,0.));
      da->SetArray(coords2->getPointer(),coords2->getNbOfElems(),0,VTK_DATA_ARRAY_FREE);//let VTK deal with double *
      coords2->accessToMemArray().setSpecificDeallocator(0);
    }
  coords->decrRef();
  vtkPoints *points=vtkPoints::New();
  ret->SetPoints(points);
  points->SetData(da);
  points->Delete();
  da->Delete();
  return ret;
}
 
vtkDataSet *MEDFileFieldRepresentationLeaves::buildVTKInstanceNoTimeInterpolation(const MEDTimeReq *tr, const MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDFileMeshes *meshes) const
{
  static const int VTK_DATA_ARRAY_FREE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  vtkDataSet *ret(0);
  //_fsp->isDataSetSupportEqualToThePreviousOne(i,globs);
  MEDCouplingAutoRefCountObjectPtr<MEDMeshMultiLev> mml(_fsp->buildFromScratchDataSetSupport(0,globs));//0=timestep Id. Make the hypothesis that support does not change 
  MEDCouplingAutoRefCountObjectPtr<MEDMeshMultiLev> mml2(mml->prepare());
  MEDMeshMultiLev *ptMML2(mml2);
  if(!_cached_ds)
    {
      MEDUMeshMultiLev *ptUMML2(dynamic_cast<MEDUMeshMultiLev *>(ptMML2));
      MEDCMeshMultiLev *ptCMML2(dynamic_cast<MEDCMeshMultiLev *>(ptMML2));
      MEDCurveLinearMeshMultiLev *ptCLMML2(dynamic_cast<MEDCurveLinearMeshMultiLev *>(ptMML2));
      
      if(ptUMML2)
        {
          ret=buildVTKInstanceNoTimeInterpolationUnstructured(ptUMML2);
        }
      else if(ptCMML2)
        {
          ret=buildVTKInstanceNoTimeInterpolationCartesian(ptCMML2);
        }
      else if(ptCLMML2)
        {
          ret=buildVTKInstanceNoTimeInterpolationCurveLinear(ptCLMML2);
        }
      else
        throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationLeaves::buildVTKInstanceNoTimeInterpolation : unrecognized mesh ! Supported for the moment unstructured, cartesian, curvelinear !");
      _cached_ds=ret->NewInstance();
      _cached_ds->ShallowCopy(ret);
    }
  else
    {
      ret=_cached_ds->NewInstance();
      ret->ShallowCopy(_cached_ds);
    }
  //
  appendFields(tr,globs,mml,meshes,ret);
  // The arrays links to mesh
  DataArrayInt *famCells(0),*numCells(0);
  bool noCpyFamCells(false),noCpyNumCells(false);
  ptMML2->retrieveFamilyIdsOnCells(famCells,noCpyFamCells);
  if(famCells)
    {
      vtkIntArray *vtkTab(vtkIntArray::New());
      vtkTab->SetNumberOfComponents(1);
      vtkTab->SetName(MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_CELL_NAME);
      if(noCpyFamCells)
        vtkTab->SetArray(famCells->getPointer(),famCells->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTab->SetArray(famCells->getPointer(),famCells->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); famCells->accessToMemArray().setSpecificDeallocator(0); }
      ret->GetCellData()->AddArray(vtkTab);
      vtkTab->Delete();
      famCells->decrRef();
    }
  ptMML2->retrieveNumberIdsOnCells(numCells,noCpyNumCells);
  if(numCells)
    {
      vtkIntArray *vtkTab(vtkIntArray::New());
      vtkTab->SetNumberOfComponents(1);
      vtkTab->SetName(MEDFileFieldRepresentationLeavesArrays::NUM_ID_CELL_NAME);
      if(noCpyNumCells)
        vtkTab->SetArray(numCells->getPointer(),numCells->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTab->SetArray(numCells->getPointer(),numCells->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); numCells->accessToMemArray().setSpecificDeallocator(0); }
      ret->GetCellData()->AddArray(vtkTab);
      vtkTab->Delete();
      numCells->decrRef();
    }
  // The arrays links to mesh
  DataArrayInt *famNodes(0),*numNodes(0);
  bool noCpyFamNodes(false),noCpyNumNodes(false);
  ptMML2->retrieveFamilyIdsOnNodes(famNodes,noCpyFamNodes);
  if(famNodes)
    {
      vtkIntArray *vtkTab(vtkIntArray::New());
      vtkTab->SetNumberOfComponents(1);
      vtkTab->SetName(MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME);
      if(noCpyFamNodes)
        vtkTab->SetArray(famNodes->getPointer(),famNodes->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTab->SetArray(famNodes->getPointer(),famNodes->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); famNodes->accessToMemArray().setSpecificDeallocator(0); }
      ret->GetPointData()->AddArray(vtkTab);
      vtkTab->Delete();
      famNodes->decrRef();
    }
  ptMML2->retrieveNumberIdsOnNodes(numNodes,noCpyNumNodes);
  if(numNodes)
    {
      vtkIntArray *vtkTab(vtkIntArray::New());
      vtkTab->SetNumberOfComponents(1);
      vtkTab->SetName(MEDFileFieldRepresentationLeavesArrays::NUM_ID_NODE_NAME);
      if(noCpyNumNodes)
        vtkTab->SetArray(numNodes->getPointer(),numNodes->getNbOfElems(),1,VTK_DATA_ARRAY_FREE);
      else
        { vtkTab->SetArray(numNodes->getPointer(),numNodes->getNbOfElems(),0,VTK_DATA_ARRAY_FREE); numNodes->accessToMemArray().setSpecificDeallocator(0); }
      ret->GetPointData()->AddArray(vtkTab);
      vtkTab->Delete();
      numNodes->decrRef();
    }
  return ret;
}

//////////////////////

MEDFileFieldRepresentationTree::MEDFileFieldRepresentationTree()
{
}

int MEDFileFieldRepresentationTree::getNumberOfLeavesArrays() const
{
  int ret(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        ret+=(*it2).getNumberOfArrays();
  return ret;
}

void MEDFileFieldRepresentationTree::assignIds() const
{
  int zeId(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        (*it2).setId(zeId);
}
void MEDFileFieldRepresentationTree::activateTheFirst() const
{
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        {
          (*it2).activateAllArrays();
          return ;
        }
}

void MEDFileFieldRepresentationTree::feedSIL(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const
{
  std::size_t it0Cnt(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++,it0Cnt++)
    {
      vtkIdType InfoOnTSId(sil->AddChild(root,edge));
      names.push_back((*it0)[0][0].getHumanReadableOverviewOfTS());
      //
      vtkIdType NbOfTSId(sil->AddChild(InfoOnTSId,edge));
      std::vector<double> ts;
      std::vector< std::pair<int,int> > dtits((*it0)[0][0].getTimeStepsInCoarseMEDFileFormat(ts));
      std::size_t nbOfTS(dtits.size());
      std::ostringstream oss3; oss3 << nbOfTS;
      names.push_back(oss3.str());
      for(std::size_t i=0;i<nbOfTS;i++)
        {
          std::ostringstream oss4; oss4 << dtits[i].first;
          vtkIdType DtId(sil->AddChild(NbOfTSId,edge));
          names.push_back(oss4.str());
          std::ostringstream oss5; oss5 << dtits[i].second;
          vtkIdType ItId(sil->AddChild(DtId,edge));
          names.push_back(oss5.str());
          std::ostringstream oss6; oss6 << ts[i];
          sil->AddChild(ItId,edge);
          names.push_back(oss6.str());
        }
      //
      std::ostringstream oss; oss << MEDFileFieldRepresentationLeavesArrays::TS_STR << it0Cnt;
      std::string tsName(oss.str());
      vtkIdType typeId0(sil->AddChild(root,edge));
      names.push_back(tsName);
      for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
        {
          std::string meshName((*it1)[0].getMeshName());
          vtkIdType typeId1(sil->AddChild(typeId0,edge));
          names.push_back(meshName);
          std::size_t it2Cnt(0);
          for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++,it2Cnt++)
            {
              std::ostringstream oss2; oss2 << MEDFileFieldRepresentationLeavesArrays::COM_SUP_STR << it2Cnt;
              std::string comSupStr(oss2.str());
              vtkIdType typeId2(sil->AddChild(typeId1,edge));
              names.push_back(comSupStr);
              (*it2).feedSIL(_ms,sil,typeId2,edge,tsName,meshName,comSupStr,names);
            } 
        }
    }
}

std::string MEDFileFieldRepresentationTree::feedSILForFamsAndGrps(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const
{
  int dummy0(0),dummy1(0),dummy2(0);
  const MEDFileFieldRepresentationLeaves& leaf(getTheSingleActivated(dummy0,dummy1,dummy2));
  std::string ret(leaf.getMeshName());
  int i(0);
  MEDFileMesh *m(0);
  for(;i<_ms->getNumberOfMeshes();i++)
    {
      m=_ms->getMeshAtPos(i);
      if(m->getName()==ret)
        break;
    }
  if(i==_ms->getNumberOfMeshes())
    throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationTree::feedSILForFamsAndGrps : internal error #0 !");
  vtkIdType typeId0(sil->AddChild(root,edge));
  names.push_back(m->getName());
  //
  vtkIdType typeId1(sil->AddChild(typeId0,edge));
  names.push_back(std::string(ROOT_OF_GRPS_IN_TREE));
  std::vector<std::string> grps(m->getGroupsNames());
  for(std::vector<std::string>::const_iterator it0=grps.begin();it0!=grps.end();it0++)
    {
      vtkIdType typeId2(sil->AddChild(typeId1,edge));
      names.push_back(*it0);
      std::vector<std::string> famsOnGrp(m->getFamiliesOnGroup((*it0).c_str()));
      for(std::vector<std::string>::const_iterator it1=famsOnGrp.begin();it1!=famsOnGrp.end();it1++)
        {
          sil->AddChild(typeId2,edge);
          names.push_back((*it1).c_str());
        }
    }
  //
  vtkIdType typeId11(sil->AddChild(typeId0,edge));
  names.push_back(std::string(ROOT_OF_FAM_IDS_IN_TREE));
  std::vector<std::string> fams(m->getFamiliesNames());
  for(std::vector<std::string>::const_iterator it00=fams.begin();it00!=fams.end();it00++)
    {
      sil->AddChild(typeId11,edge);
      int famId(m->getFamilyId((*it00).c_str()));
      std::ostringstream oss; oss << (*it00) << MEDFileFieldRepresentationLeavesArrays::ZE_SEP << famId;
      names.push_back(oss.str());
    }
  return ret;
}

const MEDFileFieldRepresentationLeavesArrays& MEDFileFieldRepresentationTree::getLeafArr(int id) const
{
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        if((*it2).containId(id))
          return (*it2).getLeafArr(id);
  throw INTERP_KERNEL::Exception("Internal error in MEDFileFieldRepresentationTree::getLeafArr !");
}

std::string MEDFileFieldRepresentationTree::getNameOf(int id) const
{
  const MEDFileFieldRepresentationLeavesArrays& elt(getLeafArr(id));
  return elt.getZeName();
}

bool MEDFileFieldRepresentationTree::getStatusOf(int id) const
{
  const MEDFileFieldRepresentationLeavesArrays& elt(getLeafArr(id));
  return elt.getStatus();
}

int MEDFileFieldRepresentationTree::getIdHavingZeName(const char *name) const
{
  int ret(-1);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        if((*it2).containZeName(name,ret))
          return ret;
  std::ostringstream msg; msg << "MEDFileFieldRepresentationTree::getIdHavingZeName : No such a name \"" << name << "\" !";
  throw INTERP_KERNEL::Exception(msg.str().c_str());
}

bool MEDFileFieldRepresentationTree::changeStatusOfAndUpdateToHaveCoherentVTKDataSet(int id, bool status) const
{
  const MEDFileFieldRepresentationLeavesArrays& elt(getLeafArr(id));
  bool ret(elt.setStatus(status));//to be implemented
  return ret;
}

int MEDFileFieldRepresentationTree::getMaxNumberOfTimeSteps() const
{
  int ret(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        ret=std::max(ret,(*it2).getNumberOfTS());
  return ret;
}

/*!
 * 
 */
void MEDFileFieldRepresentationTree::loadMainStructureOfFile(const char *fileName, bool isMEDOrSauv)
{
  if(isMEDOrSauv)
    {
      _ms=MEDFileMeshes::New(fileName);
      _fields=MEDFileFields::New(fileName,false);//false is important to not read the values
    }
  else
    {
      MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::SauvReader> sr(ParaMEDMEM::SauvReader::New(fileName));
      MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileData> mfd(sr->loadInMEDFileDS());
      _ms=mfd->getMeshes(); _ms->incrRef();
      int nbMeshes(_ms->getNumberOfMeshes());
      for(int i=0;i<nbMeshes;i++)
        {
          ParaMEDMEM::MEDFileMesh *tmp(_ms->getMeshAtPos(i));
          ParaMEDMEM::MEDFileUMesh *tmp2(dynamic_cast<ParaMEDMEM::MEDFileUMesh *>(tmp));
          if(tmp2)
            tmp2->forceComputationOfParts();
        }
      _fields=mfd->getFields();
      if((ParaMEDMEM::MEDFileFields *)_fields)
        _fields->incrRef();
    }
  if(!((ParaMEDMEM::MEDFileFields *)_fields))
    {
      _fields=BuildFieldFromMeshes(_ms);
    }
  else
    {
      AppendFieldFromMeshes(_ms,_fields);
    }
  _fields->removeFieldsWithoutAnyTimeStep();
  std::vector<std::string> meshNames(_ms->getMeshesNames());
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileFields> > fields_per_mesh(meshNames.size());
  for(std::size_t i=0;i<meshNames.size();i++)
    {
      fields_per_mesh[i]=_fields->partOfThisLyingOnSpecifiedMeshName(meshNames[i].c_str());
    }
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeFieldMultiTS > > allFMTSLeavesToDisplaySafe;
  std::size_t k(0);
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileFields> >::const_iterator fields=fields_per_mesh.begin();fields!=fields_per_mesh.end();fields++)
    {
      for(int j=0;j<(*fields)->getNumberOfFields();j++)
        {
          MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeFieldMultiTS> fmts((*fields)->getFieldAtPos((int)j));
          std::vector< MEDCouplingAutoRefCountObjectPtr< MEDFileAnyTypeFieldMultiTS > > tmp(fmts->splitDiscretizations());
          allFMTSLeavesToDisplaySafe.insert(allFMTSLeavesToDisplaySafe.end(),tmp.begin(),tmp.end());
        }
    }
  std::vector< MEDFileAnyTypeFieldMultiTS *> allFMTSLeavesToDisplay(allFMTSLeavesToDisplaySafe.size());
  for(std::size_t i=0;i<allFMTSLeavesToDisplaySafe.size();i++)
    {
      allFMTSLeavesToDisplay[i]=allFMTSLeavesToDisplaySafe[i];
    }
  std::vector< std::vector<MEDFileAnyTypeFieldMultiTS *> > allFMTSLeavesPerTimeSeries(MEDFileAnyTypeFieldMultiTS::SplitIntoCommonTimeSeries(allFMTSLeavesToDisplay));
  // memory safety part
  std::vector< std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeFieldMultiTS> > > allFMTSLeavesPerTimeSeriesSafe(allFMTSLeavesPerTimeSeries.size());
  for(std::size_t j=0;j<allFMTSLeavesPerTimeSeries.size();j++)
    {
      allFMTSLeavesPerTimeSeriesSafe[j].resize(allFMTSLeavesPerTimeSeries[j].size());
      for(std::size_t k=0;k<allFMTSLeavesPerTimeSeries[j].size();k++)
        {
          allFMTSLeavesPerTimeSeries[j][k]->incrRef();//because MEDFileAnyTypeFieldMultiTS::SplitIntoCommonTimeSeries do not increments the counter
          allFMTSLeavesPerTimeSeriesSafe[j][k]=allFMTSLeavesPerTimeSeries[j][k];
        }
    }
  // end of memory safety part
  // 1st : timesteps, 2nd : meshName, 3rd : common support
  this->_data_structure.resize(allFMTSLeavesPerTimeSeriesSafe.size());
  for(std::size_t i=0;i<allFMTSLeavesPerTimeSeriesSafe.size();i++)
    {
      vtksys_stl::vector< vtksys_stl::string > meshNamesLoc;
      std::vector< std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeFieldMultiTS> > > splitByMeshName;
      for(std::size_t j=0;j<allFMTSLeavesPerTimeSeriesSafe[i].size();j++)
        {
          std::string meshName(allFMTSLeavesPerTimeSeriesSafe[i][j]->getMeshName());
          vtksys_stl::vector< vtksys_stl::string >::iterator it(std::find(meshNamesLoc.begin(),meshNamesLoc.end(),meshName));
          if(it==meshNamesLoc.end())
            {
              meshNamesLoc.push_back(meshName);
              splitByMeshName.resize(splitByMeshName.size()+1);
              splitByMeshName.back().push_back(allFMTSLeavesPerTimeSeriesSafe[i][j]);
            }
          else
            splitByMeshName[std::distance(meshNamesLoc.begin(),it)].push_back(allFMTSLeavesPerTimeSeriesSafe[i][j]);
        }
      _data_structure[i].resize(meshNamesLoc.size());
      for(std::size_t j=0;j<splitByMeshName.size();j++)
        {
          std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileFastCellSupportComparator> > fsp;
          std::vector< MEDFileAnyTypeFieldMultiTS *> sbmn(splitByMeshName[j].size());
          for(std::size_t k=0;k<splitByMeshName[j].size();k++)
            sbmn[k]=splitByMeshName[j][k];
          //getMeshWithName does not return a newly allocated object ! It is a true get* method !
          std::vector< std::vector<MEDFileAnyTypeFieldMultiTS *> > commonSupSplit(MEDFileAnyTypeFieldMultiTS::SplitPerCommonSupport(sbmn,_ms->getMeshWithName(meshNamesLoc[j].c_str()),fsp));
          std::vector< std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileAnyTypeFieldMultiTS> > > commonSupSplitSafe(commonSupSplit.size());
          this->_data_structure[i][j].resize(commonSupSplit.size());
          for(std::size_t k=0;k<commonSupSplit.size();k++)
            {
              commonSupSplitSafe[k].resize(commonSupSplit[k].size());
              for(std::size_t l=0;l<commonSupSplit[k].size();l++)
                {
                  commonSupSplit[k][l]->incrRef();//because MEDFileAnyTypeFieldMultiTS::SplitPerCommonSupport does not increment pointers !
                  commonSupSplitSafe[k][l]=commonSupSplit[k][l];
                }
            }
          for(std::size_t k=0;k<commonSupSplit.size();k++)
            this->_data_structure[i][j][k]=MEDFileFieldRepresentationLeaves(commonSupSplitSafe[k],fsp[k]);
        }
    }
  this->removeEmptyLeaves();
  this->assignIds();
}

void MEDFileFieldRepresentationTree::removeEmptyLeaves()
{
  std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > > newSD;
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    {
      std::vector< std::vector< MEDFileFieldRepresentationLeaves > > newSD0;
      for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
        {
          std::vector< MEDFileFieldRepresentationLeaves > newSD1;
          for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
            if(!(*it2).empty())
              newSD1.push_back(*it2);
          if(!newSD1.empty())
            newSD0.push_back(newSD1);
        }
      if(!newSD0.empty())
        newSD.push_back(newSD0);
    }
}

bool MEDFileFieldRepresentationTree::IsFieldMeshRegardingInfo(const std::vector<std::string>& compInfos)
{
  if(compInfos.size()!=1)
    return false;
  return compInfos[0]==COMPO_STR_TO_LOCATE_MESH_DA;
}

std::string MEDFileFieldRepresentationTree::getDftMeshName() const
{
  return _data_structure[0][0][0].getMeshName();
}

std::vector<double> MEDFileFieldRepresentationTree::getTimeSteps(int& lev0, const TimeKeeper& tk) const
{
  int lev1,lev2;
  const MEDFileFieldRepresentationLeaves& leaf(getTheSingleActivated(lev0,lev1,lev2));
  return leaf.getTimeSteps(tk);
}

vtkDataSet *MEDFileFieldRepresentationTree::buildVTKInstance(bool isStdOrMode, double timeReq, std::string& meshName, const TimeKeeper& tk) const
{
  int lev0,lev1,lev2;
  const MEDFileFieldRepresentationLeaves& leaf(getTheSingleActivated(lev0,lev1,lev2));
  meshName=leaf.getMeshName();
  std::vector<double> ts(leaf.getTimeSteps(tk));
  std::size_t zeTimeId(0);
  if(ts.size()!=1)
    {
      std::vector<double> ts2(ts.size());
      std::transform(ts.begin(),ts.end(),ts2.begin(),std::bind2nd(std::plus<double>(),-timeReq));
      std::transform(ts2.begin(),ts2.end(),ts2.begin(),std::ptr_fun<double,double>(fabs));
      zeTimeId=std::distance(ts2.begin(),std::find_if(ts2.begin(),ts2.end(),std::bind2nd(std::less<double>(),1e-14)));
    }
  //2nd chance
  if(zeTimeId==(int)ts.size())
    zeTimeId=std::distance(ts.begin(),std::find(ts.begin(),ts.end(),timeReq));
  if(zeTimeId==(int)ts.size())
    {//OK the time requested does not fit time series given to ParaView. It is typically the case if more than one MEDReader instance are created or TimeInspector in real time mode.
      //In this case the default behaviour is taken. Keep the highest time step in this lower than timeReq.
      int pos(-1);
      double valAttachedToPos(-std::numeric_limits<double>::max());
      for(std::size_t i=0;i<ts.size();i++)
        {
          if(ts[i]<timeReq)
            {
              if(ts[i]>valAttachedToPos)
                {
                  pos=i;
                  valAttachedToPos=ts[i];
                }
            }
        }
      if(pos==-1)
        {// timeReq is lower than all time steps (ts). So let's keep the lowest time step greater than timeReq.
          valAttachedToPos=std::numeric_limits<double>::max();
          for(std::size_t i=0;i<ts.size();i++)
            {
              if(ts[i]<valAttachedToPos)
                {
                  pos=i;
                  valAttachedToPos=ts[i];
                }
            }
        }
      zeTimeId=pos;
      std::ostringstream oss; oss.precision(15); oss << "request for time " << timeReq << " but not in ";
      std::copy(ts.begin(),ts.end(),std::ostream_iterator<double>(oss,","));
      oss << " ! Keep time " << valAttachedToPos << " at pos #" << zeTimeId;
      std::cerr << oss.str() << std::endl;
    }
  MEDTimeReq *tr(0);
  if(!isStdOrMode)
    tr=new MEDStdTimeReq((int)zeTimeId);
  else
    tr=new MEDModeTimeReq(tk.getTheVectOfBool(),tk.getPostProcessedTime());
  vtkDataSet *ret(leaf.buildVTKInstanceNoTimeInterpolation(tr,_fields,_ms));
  delete tr;
  return ret;
}

const MEDFileFieldRepresentationLeaves& MEDFileFieldRepresentationTree::getTheSingleActivated(int& lev0, int& lev1, int& lev2) const
{
  int nbOfActivated(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++)
        if((*it2).isActivated())
          nbOfActivated++;
  if(nbOfActivated!=1)
    {
      std::ostringstream oss; oss << "MEDFileFieldRepresentationTree::getTheSingleActivated : Only one leaf must be activated ! Having " << nbOfActivated << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int i0(0),i1(0),i2(0);
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++,i0++)
    for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++,i1++)
      for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++,i2++)
        if((*it2).isActivated())
          {
            lev0=i0; lev1=i1; lev2=i2;
            return *it2;
          }
  throw INTERP_KERNEL::Exception("MEDFileFieldRepresentationTree::getTheSingleActivated : Internal error !");
}

void MEDFileFieldRepresentationTree::printMySelf(std::ostream& os) const
{
  int i(0);
  os << "#############################################" << std::endl;
  for(std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > >::const_iterator it0=_data_structure.begin();it0!=_data_structure.end();it0++,i++)
    {
      int j(0);
      os << "TS" << i << std::endl;
      for(std::vector< std::vector< MEDFileFieldRepresentationLeaves > >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++,j++)
        {
          int k(0);
          for(std::vector< MEDFileFieldRepresentationLeaves >::const_iterator it2=(*it1).begin();it2!=(*it1).end();it2++,k++)
            {
              if(k==0)
                os << "   " << (*it2).getMeshName() << std::endl;
              os << "      Comp" << k  << std::endl;
              (*it2).printMySelf(os);
            }
        }
    }
    os << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
}

void MEDFileFieldRepresentationTree::AppendFieldFromMeshes(const ParaMEDMEM::MEDFileMeshes *ms, ParaMEDMEM::MEDFileFields *ret)
{
  for(int i=0;i<ms->getNumberOfMeshes();i++)
    {
      MEDFileMesh *mm(ms->getMeshAtPos(i));
      std::vector<int> levs(mm->getNonEmptyLevels());
      ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileField1TS> f1tsMultiLev(ParaMEDMEM::MEDFileField1TS::New());
      MEDFileUMesh *mmu(dynamic_cast<MEDFileUMesh *>(mm));
      if(mmu)
	{
	  for(std::vector<int>::const_iterator it=levs.begin();it!=levs.end();it++)
	    {
	      std::vector<INTERP_KERNEL::NormalizedCellType> gts(mmu->getGeoTypesAtLevel(*it));
	      for(std::vector<INTERP_KERNEL::NormalizedCellType>::const_iterator gt=gts.begin();gt!=gts.end();gt++)
		{
		  ParaMEDMEM::MEDCouplingMesh *m(mmu->getDirectUndergroundSingleGeoTypeMesh(*gt));
		  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> f(ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS));
		  f->setMesh(m);
		  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble> arr(ParaMEDMEM::DataArrayDouble::New()); arr->alloc(f->getNumberOfTuplesExpected());
		  arr->setInfoOnComponent(0,std::string(COMPO_STR_TO_LOCATE_MESH_DA));
		  arr->iota();
		  f->setArray(arr);
		  f->setName(mm->getName());
		  f1tsMultiLev->setFieldNoProfileSBT(f);
		}
	    }
	  if(levs.empty())
	    {
	      std::vector<int> levsExt(mm->getNonEmptyLevelsExt());
	      if(levsExt.size()==levs.size()+1)
		{
		  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingMesh> m(mm->getGenMeshAtLevel(1));
		  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> f(ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES));
		  f->setMesh(m);
		  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble> arr(ParaMEDMEM::DataArrayDouble::New()); arr->alloc(m->getNumberOfNodes());
		  arr->setInfoOnComponent(0,std::string(COMPO_STR_TO_LOCATE_MESH_DA));
		  arr->iota(); f->setArray(arr);
		  f->setName(mm->getName());
		  f1tsMultiLev->setFieldNoProfileSBT(f);
		}
	      else
		continue;
	    }
	}
      else
	{
	  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingMesh> m(mm->getGenMeshAtLevel(0));
	  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> f(ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS));
	  f->setMesh(m);
	  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble> arr(ParaMEDMEM::DataArrayDouble::New()); arr->alloc(f->getNumberOfTuplesExpected());
	  arr->setInfoOnComponent(0,std::string(COMPO_STR_TO_LOCATE_MESH_DA));
	  arr->iota();
	  f->setArray(arr);
	  f->setName(mm->getName());
	  f1tsMultiLev->setFieldNoProfileSBT(f);
	}
      //
      ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFieldMultiTS> fmtsMultiLev(ParaMEDMEM::MEDFileFieldMultiTS::New());
      fmtsMultiLev->pushBackTimeStep(f1tsMultiLev);
      ret->pushField(fmtsMultiLev);
    }
}

ParaMEDMEM::MEDFileFields *MEDFileFieldRepresentationTree::BuildFieldFromMeshes(const ParaMEDMEM::MEDFileMeshes *ms)
{
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFields> ret(ParaMEDMEM::MEDFileFields::New());
  AppendFieldFromMeshes(ms,ret);
  return ret.retn();
}

std::vector<std::string> MEDFileFieldRepresentationTree::SplitFieldNameIntoParts(const std::string& fullFieldName, char sep)
{
  std::vector<std::string> ret;
  std::size_t pos(0);
  while(pos!=std::string::npos)
    {
      std::size_t curPos(fullFieldName.find_first_of(sep,pos));
      std::string elt(fullFieldName.substr(pos,curPos!=std::string::npos?curPos-pos:std::string::npos));
      ret.push_back(elt);
      pos=fullFieldName.find_first_not_of(sep,curPos);
    }
  return ret;
}

/*!
 * Here the non regression tests.
 * const char inp0[]="";
 * const char exp0[]="";
 * const char inp1[]="field";
 * const char exp1[]="field";
 * const char inp2[]="_________";
 * const char exp2[]="_________";
 * const char inp3[]="field_p";
 * const char exp3[]="field_p";
 * const char inp4[]="field__p";
 * const char exp4[]="field_p";
 * const char inp5[]="field_p__";
 * const char exp5[]="field_p";
 * const char inp6[]="field_p_";
 * const char exp6[]="field_p";
 * const char inp7[]="field_____EDFGEG//sdkjf_____PP_______________";
 * const char exp7[]="field_EDFGEG//sdkjf_PP";
 * const char inp8[]="field_____EDFGEG//sdkjf_____PP";
 * const char exp8[]="field_EDFGEG//sdkjf_PP";
 * const char inp9[]="_field_____EDFGEG//sdkjf_____PP_______________";
 * const char exp9[]="field_EDFGEG//sdkjf_PP";
 * const char inp10[]="___field_____EDFGEG//sdkjf_____PP_______________";
 * const char exp10[]="field_EDFGEG//sdkjf_PP";
*/
std::string MEDFileFieldRepresentationTree::PostProcessFieldName(const std::string& fullFieldName)
{
  static const char SEP('_');
  std::vector<std::string> v(SplitFieldNameIntoParts(fullFieldName,SEP));
  if(v.empty())
    return fullFieldName;//should never happen
  if(v.size()==1)
    {
      if(v[0].empty())
        return fullFieldName;
      else
        return v[0];
    }
  std::string ret(v[0]);
  for(std::size_t i=1;i<v.size();i++)
    {
      if(!v[i].empty())
        {
          if(!ret.empty())
            { ret+=SEP; ret+=v[i]; }
          else
            ret=v[i];
        }
    }
  if(ret.empty())
    return fullFieldName;
  return ret;
}

///////////

TimeKeeper::TimeKeeper(int policy):_policy(policy)
{
}

std::vector<double> TimeKeeper::getTimeStepsRegardingPolicy(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const
{
  switch(_policy)
    {
    case 0:
      return getTimeStepsRegardingPolicy0(tsPairs,ts);
    case 1:
      return getTimeStepsRegardingPolicy0(tsPairs,ts);
    default:
      throw INTERP_KERNEL::Exception("TimeKeeper::getTimeStepsRegardingPolicy : only policy 0 and 1 supported presently !");
    }
}

/*!
 * policy = 0 :
 * if all of ts are in -1e299,1e299 and different each other pairs are ignored ts taken directly.
 * if all of ts are in -1e299,1e299 but some are not different each other ts are ignored pairs used
 * if some of ts are out of -1e299,1e299 ts are ignored pairs used
 */
std::vector<double> TimeKeeper::getTimeStepsRegardingPolicy0(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const
{
  std::size_t sz(ts.size());
  bool isInHumanRange(true);
  std::set<double> s;
  for(std::size_t i=0;i<sz;i++)
    {
      s.insert(ts[i]);
      if(ts[i]<=-1e299 || ts[i]>=1e299)
        isInHumanRange=false;
    }
  if(!isInHumanRange)
    return processedUsingPairOfIds(tsPairs);
  if(s.size()!=sz)
    return processedUsingPairOfIds(tsPairs);
  _postprocessed_time=ts;
  return getPostProcessedTime();
}

/*!
 * policy = 1 :
 * idem than 0, except that ts is preaccumulated before invoking policy 0.
 */
std::vector<double> TimeKeeper::getTimeStepsRegardingPolicy1(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const
{
  std::size_t sz(ts.size());
  std::vector<double> ts2(sz);
  double acc(0.);
  for(std::size_t i=0;i<sz;i++)
    {
      ts2[i]=acc;
      acc+=ts[i];
    }
  return getTimeStepsRegardingPolicy0(tsPairs,ts2);
}

int TimeKeeper::getTimeStepIdFrom(double timeReq) const
{
  std::size_t pos(std::distance(_postprocessed_time.begin(),std::find(_postprocessed_time.begin(),_postprocessed_time.end(),timeReq)));
  return (int)pos;
}

void TimeKeeper::printSelf(std::ostream& oss) const
{
  std::size_t sz(_activated_ts.size());
  for(std::size_t i=0;i<sz;i++)
    {
      oss << "(" << i << "," << _activated_ts[i].first << "), ";
    }
}

std::vector<bool> TimeKeeper::getTheVectOfBool() const
{
  std::size_t sz(_activated_ts.size());
  std::vector<bool> ret(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      ret[i]=_activated_ts[i].first;
    }
  return ret;
}

std::vector<double> TimeKeeper::processedUsingPairOfIds(const std::vector< std::pair<int,int> >& tsPairs) const
{
  std::size_t sz(tsPairs.size());
  std::set<int> s0,s1;
  for(std::size_t i=0;i<sz;i++)
    { s0.insert(tsPairs[i].first); s1.insert(tsPairs[i].second); }
  if(s0.size()==sz)
    {
      _postprocessed_time.resize(sz);
      for(std::size_t i=0;i<sz;i++)
        _postprocessed_time[i]=(double)tsPairs[i].first;
      return getPostProcessedTime();
    }
  if(s1.size()==sz)
    {
      _postprocessed_time.resize(sz);
      for(std::size_t i=0;i<sz;i++)
        _postprocessed_time[i]=(double)tsPairs[i].second;
      return getPostProcessedTime();
    }
  //TimeKeeper::processedUsingPairOfIds : you are not a lucky guy ! All your time steps info in MEDFile are not discriminant taken one by one !
  _postprocessed_time.resize(sz);
  for(std::size_t i=0;i<sz;i++)
    _postprocessed_time[i]=(double)i;
  return getPostProcessedTime();
}

void TimeKeeper::setMaxNumberOfTimeSteps(int maxNumberOfTS)
{
  _activated_ts.resize(maxNumberOfTS);
  for(int i=0;i<maxNumberOfTS;i++)
    {
      std::ostringstream oss; oss << "000" << i;
      _activated_ts[i]=std::pair<bool,std::string>(true,oss.str());
    }
}
