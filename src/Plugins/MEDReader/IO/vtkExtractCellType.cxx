// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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
// Author : Anthony Geay

#include "vtkExtractCellType.h"
#include "MEDFileFieldRepresentationTree.hxx"
#include "MEDFileFieldOverView.hxx"

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

vtkStandardNewMacro(vtkExtractCellType);

vtkCxxSetObjectMacro(vtkExtractCellType, SIL, vtkMutableDirectedGraph);

///////////////////

class ExtractCellTypeStatus
{
public:
  ExtractCellTypeStatus():_status(false),_vtkt(-1),_mct(INTERP_KERNEL::NORM_ERROR) { }
  ExtractCellTypeStatus(int vtkt, INTERP_KERNEL::NormalizedCellType mct);
  bool isSame(int vtkt, INTERP_KERNEL::NormalizedCellType mct) const { return _vtkt==vtkt && _mct==mct; }
  bool getStatus() const { return _status; }
  void setStatus(bool status) const { _status=status; }
  void cpyStatusFrom(const ExtractCellTypeStatus& other) { _status=other._status; }
  std::string getKey() const { return _type_str; }
  const char *getKeyOfEntry() const { return _type_str.c_str(); }
  int getVTKCellType() const { return _vtkt; }
  void printMySelf(std::ostream& os) const;
  bool isSameAs(const ExtractCellTypeStatus& other) const;
  void feedSIL(vtkMutableDirectedGraph *sil, vtkIdType root, vtkVariantArray *childEdge, std::vector<std::string>& names) const;
protected:
  mutable bool _status;
  int _vtkt;
  INTERP_KERNEL::NormalizedCellType _mct;
  std::string _type_str;
};

class vtkExtractCellType::vtkExtractCellTypeInternal
{
public:
  vtkExtractCellTypeInternal():_ref_mtime(0) { }
  int getNumberOfEntries() const;
  const char *getKeyOfEntry(int i) const;
  bool getStatusOfEntryStr(const char *entry) const;
  void setStatusOfEntryStr(const char *entry, bool status) const;
  void feedSIL(vtkMutableDirectedGraph *sil) const;
  std::vector<int> getIdsToKeep() const;
  void printMySelf(std::ostream& os) const;
  bool setRefTime(vtkObject *input) const;
  // non const methods
  void loadFrom(const std::map<int,INTERP_KERNEL::NormalizedCellType>& m);
private:
  const ExtractCellTypeStatus& getEntry(const char *entry) const;
  bool checkSame(const std::map<int,INTERP_KERNEL::NormalizedCellType>& m) const;
private:
  std::vector<ExtractCellTypeStatus> _types;
  mutable unsigned long _ref_mtime;
};

bool vtkExtractCellType::vtkExtractCellTypeInternal::setRefTime(vtkObject *input) const
{
  unsigned long mtime(input->GetMTime());
  if(mtime>_ref_mtime)
    {
      _ref_mtime=mtime;
      return true;
    }
  else
    return false;
}

std::vector<int> vtkExtractCellType::vtkExtractCellTypeInternal::getIdsToKeep() const
{
  std::vector<int> ret;
  for(std::vector<ExtractCellTypeStatus>::const_iterator it=_types.begin();it!=_types.end();it++)
    {
      if((*it).getStatus())
        ret.push_back((*it).getVTKCellType());
    }
  return ret;
}

void vtkExtractCellType::vtkExtractCellTypeInternal::feedSIL(vtkMutableDirectedGraph *sil) const
{
  vtkSmartPointer<vtkVariantArray> childEdge(vtkSmartPointer<vtkVariantArray>::New());
  childEdge->InsertNextValue(0);
  vtkSmartPointer<vtkVariantArray> crossEdge(vtkSmartPointer<vtkVariantArray>::New());
  crossEdge->InsertNextValue(1);
  // CrossEdge is an edge linking hierarchies.
  vtkUnsignedCharArray* crossEdgesArray=vtkUnsignedCharArray::New();
  crossEdgesArray->SetName("CrossEdges");
  sil->GetEdgeData()->AddArray(crossEdgesArray);
  crossEdgesArray->Delete();
  std::vector<std::string> names;
  // Add global fields root
  vtkIdType root(sil->AddVertex());
  names.push_back("CellTypesTree");
  //
  for(std::vector<ExtractCellTypeStatus>::const_iterator it=_types.begin();it!=_types.end();it++)
    {
      (*it).feedSIL(sil,root,childEdge,names);
    }
  // This array is used to assign names to nodes.
  vtkStringArray *namesArray(vtkStringArray::New());
  namesArray->SetName("Names");
  namesArray->SetNumberOfTuples(sil->GetNumberOfVertices());
  sil->GetVertexData()->AddArray(namesArray);
  namesArray->Delete();
  std::vector<std::string>::const_iterator iter;
  vtkIdType cc;
  for(cc=0, iter=names.begin(); iter!=names.end(); ++iter, ++cc)
    namesArray->SetValue(cc,(*iter).c_str());
}

void vtkExtractCellType::vtkExtractCellTypeInternal::loadFrom(const std::map<int,INTERP_KERNEL::NormalizedCellType>& m)
{
  if(checkSame(m))
    return;
  //
  std::size_t sz(m.size()),ii(0);
  _types.resize(sz);
  for(std::map<int,INTERP_KERNEL::NormalizedCellType>::const_iterator it=m.begin();it!=m.end();it++,ii++)
    {
      ExtractCellTypeStatus elt((*it).first,(*it).second);
      _types[ii]=elt;
    }
}

int vtkExtractCellType::vtkExtractCellTypeInternal::getNumberOfEntries() const
{
  return (int) _types.size();
}

const char *vtkExtractCellType::vtkExtractCellTypeInternal::getKeyOfEntry(int i) const
{
  return _types[i].getKeyOfEntry();
}

bool vtkExtractCellType::vtkExtractCellTypeInternal::checkSame(const std::map<int,INTERP_KERNEL::NormalizedCellType>& m) const
{
  std::size_t sz(m.size());
  if(sz!=_types.size())
    return false;
  bool ret(true);
  std::map<int,INTERP_KERNEL::NormalizedCellType>::const_iterator it(m.begin());
  for(std::size_t i=0;i<sz && ret;i++,it++)
    ret=_types[i].isSame((*it).first,(*it).second);
  return ret;
}

const ExtractCellTypeStatus& vtkExtractCellType::vtkExtractCellTypeInternal::getEntry(const char *entry) const
{
  std::string entryCpp(entry);
  for(std::vector<ExtractCellTypeStatus>::const_iterator it0=_types.begin();it0!=_types.end();it0++)
    if(entryCpp==(*it0).getKey())
      return *it0;
  std::ostringstream oss; oss << "vtkExtractCellTypeInternal::getEntry : no such entry \"" << entry << "\"!";
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

bool vtkExtractCellType::vtkExtractCellTypeInternal::getStatusOfEntryStr(const char *entry) const
{
  const ExtractCellTypeStatus& elt(getEntry(entry));
  return elt.getStatus();
}

void vtkExtractCellType::vtkExtractCellTypeInternal::setStatusOfEntryStr(const char *entry, bool status) const
{
  const ExtractCellTypeStatus& elt(getEntry(entry));
  elt.setStatus(status);
}

void vtkExtractCellType::vtkExtractCellTypeInternal::printMySelf(std::ostream& os) const
{
  for(std::vector<ExtractCellTypeStatus>::const_iterator it0=_types.begin();it0!=_types.end();it0++)
    (*it0).printMySelf(os);
}

ExtractCellTypeStatus::ExtractCellTypeStatus(int vtkt, INTERP_KERNEL::NormalizedCellType mct):_status(false),_vtkt(vtkt),_mct(mct)
{
  std::string name(INTERP_KERNEL::CellModel::GetCellModel(mct).getRepr());
  _type_str=name.substr(5);//skip "NORM_"
}

void ExtractCellTypeStatus::printMySelf(std::ostream& os) const
{
  os << "      -" << _type_str << "(";
  if(_status)
    os << "X";
  else
    os << " ";
  os << ")" << std::endl;
}

bool ExtractCellTypeStatus::isSameAs(const ExtractCellTypeStatus& other) const
{
  return _vtkt==other._vtkt && _mct==other._mct;
}

void ExtractCellTypeStatus::feedSIL(vtkMutableDirectedGraph *sil, vtkIdType root, vtkVariantArray *childEdge, std::vector<std::string>& names) const
{
  vtkIdType InfoGeoType(sil->AddChild(root,childEdge));
  names.push_back(_type_str);
  vtkIdType InfoVTKID(sil->AddChild(InfoGeoType,childEdge));
  std::ostringstream oss; oss << _vtkt;
  names.push_back(oss.str());
}

////////////////////

vtkExtractCellType::vtkExtractCellType():SIL(NULL),Internal(new vtkExtractCellTypeInternal),InsideOut(0)
{
}

vtkExtractCellType::~vtkExtractCellType()
{
  delete this->Internal;
}

void vtkExtractCellType::SetInsideOut(int val)
{
  if(this->InsideOut!=val)
    {
      this->InsideOut=val;
      this->Modified();
    }
}

int vtkExtractCellType::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  try
    {
      //std::cerr << "########################################## vtkExtractCellType::RequestInformation ##########################################" << std::endl;
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      vtkInformation *inputInfo(inputVector[0]->GetInformationObject(0));
      vtkDataSet *input(0);
      {
	vtkDataObject *inp(inputInfo->Get(vtkDataObject::DATA_OBJECT()));
	if(vtkDataSet::SafeDownCast(inp))
	  input=vtkDataSet::SafeDownCast(inp);
	else
	  {
	    vtkMultiBlockDataSet *inputTmp(vtkMultiBlockDataSet::SafeDownCast(inp));
	    if(inputTmp)
	      {
		if(inputTmp->GetNumberOfBlocks()!=1)
		  {
		    vtkDebugMacro("vtkExtractCellType::RequestInformation : input vtkMultiBlockDataSet must contain exactly 1 block !");
		    return 0;
		  }
		vtkDataSet *blk0(vtkDataSet::SafeDownCast(inputTmp->GetBlock(0)));
		if(!blk0)
		  {
		    vtkDebugMacro("vtkExtractCellType::RequestInformation : the single block in input vtkMultiBlockDataSet must be a vtkDataSet instance !");
		    return 0;
		  }
		input=blk0;
	      }
	    else
	      {
		vtkDebugMacro("vtkExtractCellType::RequestInformation : supported input are vtkDataSet or vtkMultiBlockDataSet !");
		return 0;
	      }
	  }
      }
      if(this->Internal->setRefTime(input))
	{
	  vtkIdType nbOfCells(input->GetNumberOfCells());
	  std::map<int,INTERP_KERNEL::NormalizedCellType> m;
	  for(vtkIdType cellId=0;cellId<nbOfCells;cellId++)
	    {
	      int vtkCt(input->GetCellType(cellId));
	      const std::map<int,INTERP_KERNEL::NormalizedCellType>::const_iterator it(m.find(vtkCt));
	      if(it==m.end())
		{
		  const unsigned char *pos(std::find(ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE,ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE+ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE_LGTH,vtkCt));
		  if(pos==ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE+ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE_LGTH)
		    {
		      vtkDebugMacro("vtkExtractCellType::RequestInformation : cell #" << cellId << " has unrecognized type !");
		      return 0;
		    }
		  m[vtkCt]=(INTERP_KERNEL::NormalizedCellType)std::distance(ParaMEDMEM::MEDMeshMultiLev::PARAMEDMEM_2_VTKTYPE,pos);
            }
	    }
	  this->Internal->loadFrom(m);
	  if(this->SIL)
	    this->SIL->Delete();
	  this->SIL=vtkMutableDirectedGraph::New();
	  this->Internal->feedSIL(this->SIL);
	  //
	  outInfo->Set(vtkDataObject::SIL(),this->SIL);
	}
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkExtractCellType::RequestInformation : " << e.what() << std::endl;
      return 0;
    }
  return 1;
}

vtkDataSet *FilterFamilies(vtkDataSet *input, const std::vector<int>& idsToKeep, bool insideOut)
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
  vtkIdType nbOfCells(input->GetNumberOfCells());
  vtkCharArray *zeSelection(vtkCharArray::New());
  zeSelection->SetName(ZE_SELECTION_ARR_NAME);
  zeSelection->SetNumberOfComponents(1);
  char *pt(new char[nbOfCells]);
  zeSelection->SetArray(pt,nbOfCells,0,VTK_DATA_ARRAY_DELETE);
  std::fill(pt,pt+nbOfCells,0);
  std::vector<bool> pt2(nbOfCells,false);
  for(std::vector<int>::const_iterator it=idsToKeep.begin();it!=idsToKeep.end();it++)
    {
      for(vtkIdType ii=0;ii<nbOfCells;ii++)
        {
          if(input->GetCellType(ii)==*it)
            pt2[ii]=true;
        }
    }
  for(int ii=0;ii<nbOfCells;ii++)
    if(pt2[ii])
      pt[ii]=2;
  int idx(output->GetCellData()->AddArray(zeSelection));
  output->GetCellData()->SetActiveAttribute(idx,vtkDataSetAttributes::SCALARS);
  output->GetCellData()->CopyScalarsOff();
  zeSelection->Delete();
  //
  thres->SetInputArrayToProcess(idx,0,0,"vtkDataObject::FIELD_ASSOCIATION_CELLS",ZE_SELECTION_ARR_NAME);
  thres->Update();
  vtkUnstructuredGrid *zeComputedOutput(thres->GetOutput());
  zeComputedOutput->GetCellData()->RemoveArray(idx);
  output->Delete();
  zeComputedOutput->Register(0);
  return zeComputedOutput;
}

int vtkExtractCellType::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  try
    {
      //std::cerr << "########################################## vtkExtractCellType::RequestData        ##########################################" << std::endl;
      vtkInformation* inputInfo=inputVector[0]->GetInformationObject(0);
      vtkDataSet *input(vtkDataSet::SafeDownCast(inputInfo->Get(vtkDataObject::DATA_OBJECT())));
      vtkInformation *info(input->GetInformation());
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      vtkDataSet *output(vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())));
      std::vector<int> idsToKeep(this->Internal->getIdsToKeep());
      vtkDataSet *tryOnCell(FilterFamilies(input,idsToKeep,this->InsideOut));
      // first shrink the input
      output->ShallowCopy(tryOnCell);
      tryOnCell->Delete();
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkExtractCellType::RequestData : " << e.what() << std::endl;
      return 0;
    }
  return 1;
}

int vtkExtractCellType::GetSILUpdateStamp()
{
  return this->SILTime;
}

void vtkExtractCellType::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

int vtkExtractCellType::GetNumberOfGeoTypesArrays()
{
  int ret(this->Internal->getNumberOfEntries());
  //std::cerr << "vtkExtractCellType::GetNumberOfGeoTypesArrays() -> " << ret << std::endl;
  return ret;
}

const char *vtkExtractCellType::GetGeoTypesArrayName(int index)
{
  const char *ret(this->Internal->getKeyOfEntry(index));
  //std::cerr << "vtkExtractCellType::GetGeoTypesArrayName(" << index << ") -> " << ret << std::endl;
  return ret;
}

int vtkExtractCellType::GetGeoTypesArrayStatus(const char *name)
{
  int ret((int)this->Internal->getStatusOfEntryStr(name));
  //std::cerr << "vtkExtractCellType::GetGeoTypesArrayStatus(" << name << ") -> " << ret << std::endl;
  return ret;
}

void vtkExtractCellType::SetGeoTypesStatus(const char *name, int status)
{
  //std::cerr << "vtkExtractCellType::SetGeoTypesStatus(" << name << "," << status << ")" << std::endl;
  this->Internal->setStatusOfEntryStr(name,(bool)status);
  if(std::string(name)==GetGeoTypesArrayName(GetNumberOfGeoTypesArrays()-1))
    {
      this->Modified();
      //this->Internal->printMySelf(std::cerr);
    }
}
