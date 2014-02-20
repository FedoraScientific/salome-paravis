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

#include "vtkExtractGroup.h"
#include "MEDFileFieldRepresentationTree.hxx"

#include "vtkAdjacentVertexIterator.h"
#include "vtkIntArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnstructuredGrid.h"
#include  "vtkMultiBlockDataSet.h"

#include "vtkInformationStringKey.h"
#include "vtkAlgorithmOutput.h"
#include "vtkObjectFactory.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkDataSet.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataArraySelection.h"
#include "vtkTimeStamp.h"
#include "vtkInEdgeIterator.h"
#include "vtkInformationDataObjectKey.h"
#include "vtkExecutive.h"
#include "vtkVariantArray.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkDataObjectTreeIterator.h"
#include "vtkThreshold.h"

#include <map>
#include <deque>

vtkStandardNewMacro(vtkExtractGroup);

vtkCxxSetObjectMacro(vtkExtractGroup, SIL, vtkMutableDirectedGraph);

///////////////////

class ExtractGroupStatus
{
public:
  ExtractGroupStatus():_status(false) { }
  ExtractGroupStatus(const char *name);
  bool getStatus() const { return _status; }
  void setStatus(bool status) { _status=status; }
  void cpyStatusFrom(const ExtractGroupStatus& other) { _status=other._status; }
  std::string getName() const { return _name; }
  const char *getKeyOfEntry() const { return _ze_key_name.c_str(); }
  virtual void printMySelf(std::ostream& os) const;
  virtual bool isSameAs(const ExtractGroupStatus& other) const;
protected:
bool _status;
std::string _name;
std::string _ze_key_name;
};

class ExtractGroupGrp : public ExtractGroupStatus
{
public:
  ExtractGroupGrp(const char *name):ExtractGroupStatus(name) { std::ostringstream oss; oss << START << name; _ze_key_name=oss.str(); }
  void setFamilies(const std::vector<std::string>& fams) { _fams=fams; }
  const std::vector<std::string>& getFamiliesLyingOn() const { return _fams; }
  bool isSameAs(const ExtractGroupGrp& other) const;
public:
  static const char START[];
  std::vector<std::string> _fams;
};

class ExtractGroupFam : public ExtractGroupStatus
{
public:
  ExtractGroupFam(const char *name);
  void printMySelf(std::ostream& os) const;
  void fillIdsToKeep(std::set<int>& s) const;
  int getId() const { return _id; }
  bool isSameAs(const ExtractGroupFam& other) const;
public:
  static const char START[];
private:
  int _id;
};

class vtkExtractGroup::vtkExtractGroupInternal
{
public:
  void loadFrom(vtkMutableDirectedGraph *sil);
  int getNumberOfEntries() const;
  const char *getKeyOfEntry(int i) const;
  bool getStatusOfEntryStr(const char *entry) const;
  void setStatusOfEntryStr(const char *entry, bool status);
  void printMySelf(std::ostream& os) const;
  std::set<int> getIdsToKeep() const;
  int getIdOfFamily(const std::string& famName) const;
  static bool IsInformationOK(vtkInformation *info);
private:
  std::map<std::string,int> computeFamStrIdMap() const;
  const ExtractGroupStatus& getEntry(const char *entry) const;
  ExtractGroupStatus& getEntry(const char *entry);
private:
  std::vector<ExtractGroupGrp> _groups;
  std::vector<ExtractGroupFam> _fams;
};

const char ExtractGroupGrp::START[]="GRP_";

const char ExtractGroupFam::START[]="FAM_";

ExtractGroupStatus::ExtractGroupStatus(const char *name):_status(false),_name(name)
{
}

void ExtractGroupStatus::printMySelf(std::ostream& os) const
{
  os << "      -" << _ze_key_name << "(";
  if(_status)
    os << "X";
  else
    os << " ";
  os << ")" << std::endl;
}

bool ExtractGroupStatus::isSameAs(const ExtractGroupStatus& other) const
{
  return _name==other._name && _ze_key_name==other._ze_key_name;
}

bool ExtractGroupGrp::isSameAs(const ExtractGroupGrp& other) const
{
  bool ret(ExtractGroupStatus::isSameAs(other));
  if(ret)
    return _fams==other._fams;
  else
    return false;
}

bool vtkExtractGroup::vtkExtractGroupInternal::IsInformationOK(vtkInformation *info)
{
  if(!info->Has(vtkDataObject::SIL()))
    return false;
  vtkMutableDirectedGraph *sil(vtkMutableDirectedGraph::SafeDownCast(info->Get(vtkDataObject::SIL())));
  if(!sil)
    return false;
  int idNames(0);
  vtkAbstractArray *verticesNames(sil->GetVertexData()->GetAbstractArray("Names",idNames));
  vtkStringArray *verticesNames2(vtkStringArray::SafeDownCast(verticesNames));
  if(!verticesNames2)
    return false;
  for(int i=0;i<verticesNames2->GetNumberOfValues();i++)
    {
      vtkStdString &st(verticesNames2->GetValue(i));
      if(st=="MeshesFamsGrps")
        return true;
    }
  return false;
}

void vtkExtractGroup::vtkExtractGroupInternal::loadFrom(vtkMutableDirectedGraph *sil)
{
  std::vector<ExtractGroupGrp> oldGrps(_groups); _groups.clear();
  std::vector<ExtractGroupFam> oldFams(_fams); _fams.clear();
  int idNames(0);
  vtkAbstractArray *verticesNames(sil->GetVertexData()->GetAbstractArray("Names",idNames));
  vtkStringArray *verticesNames2(vtkStringArray::SafeDownCast(verticesNames));
  vtkIdType id0;
  bool found(false);
  for(int i=0;i<verticesNames2->GetNumberOfValues();i++)
    {
      vtkStdString &st(verticesNames2->GetValue(i));
      if(st=="MeshesFamsGrps")
        {
          id0=i;
          found=true;
        }
    }
  if(!found)
    throw INTERP_KERNEL::Exception("There is an internal error ! The tree on server side has not the expected look !");
  vtkAdjacentVertexIterator *it0(vtkAdjacentVertexIterator::New());
  sil->GetAdjacentVertices(id0,it0);
  int kk(0),ll(0);
  while(it0->HasNext())
    {
      vtkIdType id1(it0->Next());
      std::string meshName((const char *)verticesNames2->GetValue(id1));
      vtkAdjacentVertexIterator *it1(vtkAdjacentVertexIterator::New());
      sil->GetAdjacentVertices(id1,it1);
      vtkIdType idZeGrps(it1->Next());//zeGroups
      vtkAdjacentVertexIterator *itGrps(vtkAdjacentVertexIterator::New());
      sil->GetAdjacentVertices(idZeGrps,itGrps);
      while(itGrps->HasNext())
        {
          vtkIdType idg(itGrps->Next());
          ExtractGroupGrp grp((const char *)verticesNames2->GetValue(idg));
          vtkAdjacentVertexIterator *itGrps2(vtkAdjacentVertexIterator::New());
          sil->GetAdjacentVertices(idg,itGrps2);
          std::vector<std::string> famsOnGroup;
          while(itGrps2->HasNext())
            {
              vtkIdType idgf(itGrps2->Next());
              famsOnGroup.push_back(std::string((const char *)verticesNames2->GetValue(idgf)));
            }
          grp.setFamilies(famsOnGroup);
          itGrps2->Delete();
          _groups.push_back(grp);
        }
      itGrps->Delete();
      vtkIdType idZeFams(it1->Next());//zeFams
      it1->Delete();
      vtkAdjacentVertexIterator *itFams(vtkAdjacentVertexIterator::New());
      sil->GetAdjacentVertices(idZeFams,itFams);
      while(itFams->HasNext())
        {
          vtkIdType idf(itFams->Next());
          ExtractGroupFam fam((const char *)verticesNames2->GetValue(idf));
          _fams.push_back(fam);
        }
      itFams->Delete();
    }
  it0->Delete(); 
  //
  std::size_t szg(_groups.size()),szf(_fams.size());
  if(szg==oldGrps.size() && szf==oldFams.size())
    {
      bool isSame(true);
      for(std::size_t i=0;i<szg && isSame;i++)
        isSame=_groups[i].isSameAs(oldGrps[i]);
      for(std::size_t i=0;i<szf && isSame;i++)
        isSame=_fams[i].isSameAs(oldFams[i]);
      if(isSame)
        {
          for(std::size_t i=0;i<szg;i++)
            _groups[i].cpyStatusFrom(oldGrps[i]);
          for(std::size_t i=0;i<szf;i++)
            _fams[i].cpyStatusFrom(oldFams[i]);
        }
    }
}

int vtkExtractGroup::vtkExtractGroupInternal::getNumberOfEntries() const
{
  std::size_t sz0(_groups.size()),sz1(_fams.size());
  return (int)(sz0+sz1);
}

const char *vtkExtractGroup::vtkExtractGroupInternal::getKeyOfEntry(int i) const
{
  int sz0((int)_groups.size());
  if(i>=0 && i<sz0)
    return _groups[i].getKeyOfEntry();
  else
    return _fams[i-sz0].getKeyOfEntry();
}

bool vtkExtractGroup::vtkExtractGroupInternal::getStatusOfEntryStr(const char *entry) const
{
  const ExtractGroupStatus& elt(getEntry(entry));
  return elt.getStatus();
}

void vtkExtractGroup::vtkExtractGroupInternal::setStatusOfEntryStr(const char *entry, bool status)
{
  ExtractGroupStatus& elt(getEntry(entry));
  elt.setStatus(status);
}

const ExtractGroupStatus& vtkExtractGroup::vtkExtractGroupInternal::getEntry(const char *entry) const
{
  std::string entryCpp(entry);
  for(std::vector<ExtractGroupGrp>::const_iterator it0=_groups.begin();it0!=_groups.end();it0++)
    if(entryCpp==(*it0).getKeyOfEntry())
      return *it0;
  for(std::vector<ExtractGroupFam>::const_iterator it0=_fams.begin();it0!=_fams.end();it0++)
    if(entryCpp==(*it0).getKeyOfEntry())
      return *it0;
  std::ostringstream oss; oss << "vtkExtractGroupInternal::getEntry : no such entry \"" << entry << "\"!";
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

ExtractGroupStatus& vtkExtractGroup::vtkExtractGroupInternal::getEntry(const char *entry)
{
  std::string entryCpp(entry);
  for(std::vector<ExtractGroupGrp>::iterator it0=_groups.begin();it0!=_groups.end();it0++)
    if(entryCpp==(*it0).getKeyOfEntry())
      return *it0;
  for(std::vector<ExtractGroupFam>::iterator it0=_fams.begin();it0!=_fams.end();it0++)
    if(entryCpp==(*it0).getKeyOfEntry())
      return *it0;
  std::ostringstream oss; oss << "vtkExtractGroupInternal::getEntry : no such entry \"" << entry << "\"!";
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

void vtkExtractGroup::vtkExtractGroupInternal::printMySelf(std::ostream& os) const
{
  os << "Groups :" << std::endl;
  for(std::vector<ExtractGroupGrp>::const_iterator it0=_groups.begin();it0!=_groups.end();it0++)
    (*it0).printMySelf(os);
  os << "Families :" << std::endl;
  for(std::vector<ExtractGroupFam>::const_iterator it0=_fams.begin();it0!=_fams.end();it0++)
    (*it0).printMySelf(os);
}

int vtkExtractGroup::vtkExtractGroupInternal::getIdOfFamily(const std::string& famName) const
{
  for(std::vector<ExtractGroupFam>::const_iterator it=_fams.begin();it!=_fams.end();it++)
    {
      if((*it).getName()==famName)
        return (*it).getId();
    }
}

ExtractGroupFam::ExtractGroupFam(const char *name):ExtractGroupStatus(name),_id(0)
{
  std::size_t pos(_name.find(MEDFileFieldRepresentationLeavesArrays::ZE_SEP));
  std::string name0(_name.substr(0,pos)),name1(_name.substr(pos+strlen(MEDFileFieldRepresentationLeavesArrays::ZE_SEP)));
  std::istringstream iss(name1);
  iss >> _id;
  std::ostringstream oss; oss << START << name; _ze_key_name=oss.str(); _name=name0;
}

bool ExtractGroupFam::isSameAs(const ExtractGroupFam& other) const
{
  bool ret(ExtractGroupStatus::isSameAs(other));
  if(ret)
    return _id==other._id;
  else
    return false;
}

void ExtractGroupFam::printMySelf(std::ostream& os) const
{
  os << "      -" << _ze_key_name << " famName : \"" << _name << "\" id : " << _id << " (";
  if(_status)
    os << "X";
  else
    os << " ";
  os << ")" << std::endl;
}

void ExtractGroupFam::fillIdsToKeep(std::set<int>& s) const
{
  s.insert(_id);
}

std::set<int> vtkExtractGroup::vtkExtractGroupInternal::getIdsToKeep() const
{
  std::map<std::string,int> m(this->computeFamStrIdMap());
  std::set<int> s;
  for(std::vector<ExtractGroupGrp>::const_iterator it0=_groups.begin();it0!=_groups.end();it0++)
    {
      if((*it0).getStatus())
        {
          const std::vector<std::string>& fams((*it0).getFamiliesLyingOn());
          for(std::vector<std::string>::const_iterator it1=fams.begin();it1!=fams.end();it1++)
            {
              std::map<std::string,int>::iterator it2(m.find((*it1)));
              if(it2!=m.end())
                s.insert((*it2).second);
            }
        }
     }
  for(std::vector<ExtractGroupFam>::const_iterator it0=_fams.begin();it0!=_fams.end();it0++)
    if((*it0).getStatus())
      (*it0).fillIdsToKeep(s);
  return s;
}

std::map<std::string,int> vtkExtractGroup::vtkExtractGroupInternal::computeFamStrIdMap() const
{
  std::map<std::string,int> ret;
  for(std::vector<ExtractGroupFam>::const_iterator it0=_fams.begin();it0!=_fams.end();it0++)
    ret[(*it0).getName()]=(*it0).getId();
  return ret;
}

////////////////////

vtkExtractGroup::vtkExtractGroup():SIL(NULL),Internal(new vtkExtractGroupInternal),InsideOut(0)
{
}

vtkExtractGroup::~vtkExtractGroup()
{
  delete this->Internal;
}

void vtkExtractGroup::SetInsideOut(int val)
{
  if(this->InsideOut!=val)
    {
      this->InsideOut=val;
      this->Modified();
    }
}

int vtkExtractGroup::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  try
    {
      //std::cerr << "########################################## vtkExtractGroup::RequestInformation ##########################################" << std::endl;
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      vtkInformation *inputInfo(inputVector[0]->GetInformationObject(0));//unfortunately inputInfo->Has(vtkDataObject::SIL) returns false... use executive to find it !
      //
      vtkExecutive *exe(GetExecutive());
      vtkAlgorithm *alg(this);
      vtkInformation *infoOnSIL(alg->GetOutputInformation(0));
      while(!vtkExtractGroup::vtkExtractGroupInternal::IsInformationOK(infoOnSIL))// skipping vtkPVPostFilter
	{
	  if(exe->GetNumberOfInputConnections(0)<1)
	    {
	      vtkErrorMacro("No SIL Data available ! The source of this filter must be MEDReader !");
	      return 0;
	    }
	  vtkExecutive *exe2(exe->GetInputExecutive(0,0));
	  //
	  alg=exe2->GetAlgorithm(); exe=exe2; infoOnSIL=alg->GetOutputInformation(0);
	}
      //
      this->SetSIL(vtkMutableDirectedGraph::SafeDownCast(infoOnSIL->Get(vtkDataObject::SIL())));
      this->Internal->loadFrom(this->SIL);
      //this->Internal->printMySelf(std::cerr);
      outInfo->Set(vtkDataObject::SIL(),this->SIL);
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkExtractGroup::RequestInformation : " << e.what() << std::endl;
      return 0;
    }
  return 1;
}

template<class CellPointExtractor>
vtkDataSet *FilterFamilies(vtkDataSet *input, const std::set<int>& idsToKeep, bool insideOut, const char *arrNameOfFamilyField,
                           const char *associationForThreshold, bool& catchAll, bool& catchSmth)
{
  static const int VTK_DATA_ARRAY_DELETE=vtkDataArrayTemplate<double>::VTK_DATA_ARRAY_DELETE;
  static const char ZE_SELECTION_ARR_NAME[]="@@ZeSelection@@";
  vtkDataSet *output(input->NewInstance());
  output->ShallowCopy(input);
  vtkSmartPointer<vtkThreshold> thres(vtkSmartPointer<vtkThreshold>::New());
  thres->SetInputData(output);
  vtkDataSetAttributes *dscIn(input->GetCellData()),*dscIn2(input->GetPointData());
  vtkDataSetAttributes *dscOut(output->GetCellData()),*dscOut2(output->GetPointData());
  //
  double vMin(insideOut==0?1.:0.),vMax(insideOut==0?2.:1.);
  thres->ThresholdBetween(vMin,vMax);
  // OK for the output 
  //
  CellPointExtractor cpe2(input);
  vtkDataArray *da(cpe2.Get()->GetScalars(arrNameOfFamilyField));
  if(!da)
    return 0;
  std::string daName(da->GetName());
  vtkIntArray *dai(vtkIntArray::SafeDownCast(da));
  if(daName!=arrNameOfFamilyField || !dai)
    return 0;
  //
  int nbOfTuples(dai->GetNumberOfTuples());
  vtkCharArray *zeSelection(vtkCharArray::New());
  zeSelection->SetName(ZE_SELECTION_ARR_NAME);
  zeSelection->SetNumberOfComponents(1);
  char *pt(new char[nbOfTuples]);
  zeSelection->SetArray(pt,nbOfTuples,0,VTK_DATA_ARRAY_DELETE);
  const int *inPtr(dai->GetPointer(0));
  std::fill(pt,pt+nbOfTuples,0);
  catchAll=true; catchSmth=false;
  std::vector<bool> pt2(nbOfTuples,false);
  for(std::set<int>::const_iterator it=idsToKeep.begin();it!=idsToKeep.end();it++)
    {
      bool catchFid(false);
      for(int i=0;i<nbOfTuples;i++)
        if(inPtr[i]==*it)
          { pt2[i]=true; catchFid=true; }
      if(!catchFid)
        catchAll=false;
      else
        catchSmth=true;
    }
  for(int ii=0;ii<nbOfTuples;ii++)
    if(pt2[ii])
      pt[ii]=2;
  CellPointExtractor cpe3(output);
  int idx(cpe3.Get()->AddArray(zeSelection));
  cpe3.Get()->SetActiveAttribute(idx,vtkDataSetAttributes::SCALARS);
  cpe3.Get()->CopyScalarsOff();
  zeSelection->Delete();
  //
  thres->SetInputArrayToProcess(idx,0,0,associationForThreshold,ZE_SELECTION_ARR_NAME);
  thres->Update();
  vtkUnstructuredGrid *zeComputedOutput(thres->GetOutput());
  CellPointExtractor cpe(zeComputedOutput);
  cpe.Get()->RemoveArray(idx);
  output->Delete();
  zeComputedOutput->Register(0);
  return zeComputedOutput;
}

class CellExtractor
{
public:
  CellExtractor(vtkDataSet *ds):_ds(ds) { }
  vtkDataSetAttributes *Get() { return _ds->GetCellData(); }
private:
  vtkDataSet *_ds;
};

class PointExtractor
{
public:
  PointExtractor(vtkDataSet *ds):_ds(ds) { }
  vtkDataSetAttributes *Get() { return _ds->GetPointData(); }
private:
  vtkDataSet *_ds;
};

int vtkExtractGroup::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  try
    {
      //std::cerr << "########################################## vtkExtractGroup::RequestData        ##########################################" << std::endl;
      vtkInformation* inputInfo=inputVector[0]->GetInformationObject(0);
      vtkDataSet *input(vtkDataSet::SafeDownCast(inputInfo->Get(vtkDataObject::DATA_OBJECT())));
      vtkInformation *info(input->GetInformation());
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      vtkDataSet *output(vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())));
      std::set<int> idsToKeep(this->Internal->getIdsToKeep());
      // first shrink the input
      bool catchAll,catchSmth;
      vtkDataSet *tryOnCell(FilterFamilies<CellExtractor>(input,idsToKeep,this->InsideOut,
							  MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_CELL_NAME,"vtkDataObject::FIELD_ASSOCIATION_CELLS",catchAll,catchSmth));
      if(tryOnCell)
	{
	  if(catchAll)
	    {
	      output->ShallowCopy(tryOnCell);
	      tryOnCell->Delete();//
	      return 1;
	    }
	  else
	    {
	      if(catchSmth)
		{
		  vtkDataSet *tryOnNode(FilterFamilies<PointExtractor>(tryOnCell,idsToKeep,this->InsideOut,
								       MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME,"vtkDataObject::FIELD_ASSOCIATION_POINTS",catchAll,catchSmth));
		  if(tryOnNode && catchSmth)
		    {
		      output->ShallowCopy(tryOnNode);
		      tryOnCell->Delete();
		      tryOnNode->Delete();//
		      return 1;
		    }
		  else
		    {
		      if(tryOnNode)
			tryOnNode->Delete();
		      output->ShallowCopy(tryOnCell);
		      tryOnCell->Delete();
		      return 1;
		    }
		}
	      else
		{
		  vtkDataSet *tryOnNode(FilterFamilies<PointExtractor>(input,idsToKeep,this->InsideOut,
								       MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME,"vtkDataObject::FIELD_ASSOCIATION_POINTS",catchAll,catchSmth));
		  if(tryOnNode)
		    {
		      tryOnCell->Delete();
		      output->ShallowCopy(tryOnNode);
		      tryOnNode->Delete();
		      return 1;
		    }
		  else
		    {
		      output->ShallowCopy(tryOnCell);
		      tryOnCell->Delete();
		      return 0;
		    }
		}
	    }
	}
      else
	{
	  vtkDataSet *tryOnNode(FilterFamilies<PointExtractor>(tryOnCell,idsToKeep,this->InsideOut,
							       MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME,"vtkDataObject::FIELD_ASSOCIATION_POINTS",catchAll,catchSmth));
	  if(tryOnNode)
	    {
	      output->ShallowCopy(tryOnNode);
	      tryOnNode->Delete();//
	      return 1;
	    }
	  else
	    {
	      std::ostringstream oss; oss << "vtkExtractGroup::RequestData : The integer array with name \""<< MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_CELL_NAME;
	      oss << "\" or \"" << MEDFileFieldRepresentationLeavesArrays::FAMILY_ID_NODE_NAME << "\" does not exist ! The extraction of group and/or family is not possible !";
	      if(this->HasObserver("ErrorEvent") )
		this->InvokeEvent("ErrorEvent",const_cast<char *>(oss.str().c_str()));
	      else
		vtkOutputWindowDisplayErrorText(const_cast<char *>(oss.str().c_str()));
	      vtkObject::BreakOnError();
	      return 0;
	    }
	}
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkExtractGroup::RequestData : " << e.what() << std::endl;
      return 0;
    }
}

int vtkExtractGroup::GetSILUpdateStamp()
{
  return this->SILTime;
}

void vtkExtractGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

int vtkExtractGroup::GetNumberOfGroupsFlagsArrays()
{
  int ret(this->Internal->getNumberOfEntries());
  //std::cerr << "vtkExtractGroup::GetNumberOfFieldsTreeArrays() -> " << ret << std::endl;
  return ret;
}

const char *vtkExtractGroup::GetGroupsFlagsArrayName(int index)
{
  const char *ret(this->Internal->getKeyOfEntry(index));
  //std::cerr << "vtkExtractGroup::GetFieldsTreeArrayName(" << index << ") -> " << ret << std::endl;
  return ret;
}

int vtkExtractGroup::GetGroupsFlagsArrayStatus(const char *name)
{
  int ret((int)this->Internal->getStatusOfEntryStr(name));
  //std::cerr << "vtkExtractGroup::GetGroupsFlagsArrayStatus(" << name << ") -> " << ret << std::endl;
  return ret;
}

void vtkExtractGroup::SetGroupsFlagsStatus(const char *name, int status)
{
  //std::cerr << "vtkExtractGroup::SetFieldsStatus(" << name << "," << status << ")" << std::endl;
  this->Internal->setStatusOfEntryStr(name,(bool)status);
  if(std::string(name)==GetGroupsFlagsArrayName(GetNumberOfGroupsFlagsArrays()-1))
     this->Modified();
  //this->Internal->printMySelf(std::cerr);
}
