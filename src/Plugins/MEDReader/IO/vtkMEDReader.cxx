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

#include "vtkMEDReader.h"
#include "vtkGenerateVectors.h"

#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"
#include "vtkDataSetAttributes.h"
#include "vtkStringArray.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkInformationStringKey.h"
//
#include "vtkUnsignedCharArray.h"
#include "vtkInformationVector.h"
#include "vtkSmartPointer.h"
#include "vtkVariantArray.h"
#include "vtkExecutive.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMultiTimeStepAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellType.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"

#include "MEDFileFieldRepresentationTree.hxx"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

/*!
 * This class stores properties in loading state mode (pvsm) when the MED file has not been read yet.
 * The file is not read beacause FileName has not been informed yet ! So this class stores properties of vtkMEDReader instance that 
 * owns it and wait the vtkMEDReader::SetFileName to apply properties afterwards.
 */
class PropertyKeeper
{
public:
  PropertyKeeper(vtkMEDReader *master):_master(master),IsGVActivated(false),GVValue(0),IsCMActivated(false),CMValue(0) { }
  void assignPropertiesIfNeeded();
  bool arePropertiesOnTreeToSetAfter() const;
  //
  void pushFieldStatusEntry(const char* name, int status);
  void pushGenerateVectorsValue(int value);
  void pushChangeModeValue(int value);
  void pushTimesFlagsStatusEntry(const char* name, int status);
protected:
  // pool of pairs to assign in SetFieldsStatus if needed. The use case is the load using pvsm.
  std::vector< std::pair<std::string,int> > SetFieldsStatusPairs;
  // generate vector
  bool IsGVActivated;
  int GVValue;
  // change mode
  bool IsCMActivated;
  int CMValue;
  //
  std::vector< std::pair<std::string,int> > TimesFlagsStatusPairs;
  vtkMEDReader *_master;
};

void PropertyKeeper::assignPropertiesIfNeeded()
{
  if(!this->SetFieldsStatusPairs.empty())
    {
      for(std::vector< std::pair<std::string,int> >::const_iterator it=this->SetFieldsStatusPairs.begin();it!=this->SetFieldsStatusPairs.end();it++)
        _master->SetFieldsStatus((*it).first.c_str(),(*it).second);
      this->SetFieldsStatusPairs.clear();
    }
  if(!this->TimesFlagsStatusPairs.empty())
    {
      for(std::vector< std::pair<std::string,int> >::const_iterator it=this->TimesFlagsStatusPairs.begin();it!=this->TimesFlagsStatusPairs.end();it++)
        _master->SetTimesFlagsStatus((*it).first.c_str(),(*it).second);
      this->TimesFlagsStatusPairs.clear();
    }
  if(this->IsGVActivated)
    {
      _master->GenerateVectors(this->GVValue);
      this->IsGVActivated=false;
    }
  if(this->IsCMActivated)
    {
      _master->ChangeMode(this->CMValue);
      this->IsCMActivated=false;
    }
}

void PropertyKeeper::pushFieldStatusEntry(const char* name, int status)
{
  bool found(false);
  for(std::vector< std::pair<std::string,int> >::const_iterator it=this->SetFieldsStatusPairs.begin();it!=this->SetFieldsStatusPairs.end() && !found;it++)
    found=(*it).first==name;
  if(!found)
    this->SetFieldsStatusPairs.push_back(std::pair<std::string,int>(name,status));
}

void PropertyKeeper::pushTimesFlagsStatusEntry(const char* name, int status)
{
  bool found(false);
  for(std::vector< std::pair<std::string,int> >::const_iterator it=this->TimesFlagsStatusPairs.begin();it!=this->TimesFlagsStatusPairs.end() && !found;it++)
    found=(*it).first==name;
  if(!found)
    this->TimesFlagsStatusPairs.push_back(std::pair<std::string,int>(name,status));
}

void PropertyKeeper::pushGenerateVectorsValue(int value)
{
  this->IsGVActivated=true;
  this->GVValue=value;
}

void PropertyKeeper::pushChangeModeValue(int value)
{
  this->IsCMActivated=true;
  this->CMValue=value;
}

bool PropertyKeeper::arePropertiesOnTreeToSetAfter() const
{
  return !SetFieldsStatusPairs.empty();
}

class vtkMEDReader::vtkMEDReaderInternal
{

public:
  vtkMEDReaderInternal(vtkMEDReader *master):TK(0),IsMEDOrSauv(true),IsStdOrMode(false),GenerateVect(false),SIL(0),LastLev0(-1),FirstCall0(2),PK(master)
  {
  }
  
  bool PluginStart0()
  {
    if(FirstCall0==0)
      return false;
    FirstCall0--;
    return true;
  }
  
  ~vtkMEDReaderInternal()
  {
    if(this->SIL)
      this->SIL->Delete();
  }
public:
  MEDFileFieldRepresentationTree Tree;
  TimeKeeper TK;
  std::string FileName;
  //when true the file is MED file. when false it is a Sauv file
  bool IsMEDOrSauv;
  //when false -> std, true -> mode. By default std (false).
  bool IsStdOrMode;
  //when false -> do nothing. When true cut off or extend to nbOfCompo=3 vector arrays.
  bool GenerateVect;
  std::string DftMeshName;
  // Store the vtkMutableDirectedGraph that represents links between family, groups and cell types
  vtkMutableDirectedGraph* SIL;
  // store the lev0 id in Tree corresponding to the TIME_STEPS in the pipeline.
  int LastLev0;
  // The property keeper is usable only in pvsm mode.
  PropertyKeeper PK;
private:
  unsigned char FirstCall0;
};

vtkStandardNewMacro(vtkMEDReader);

vtkMEDReader::vtkMEDReader():Internal(new vtkMEDReaderInternal(this))
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

vtkMEDReader::~vtkMEDReader()
{
  delete this->Internal;
}

void vtkMEDReader::Reload(int a)
{
  if(a==0)
    return;
  std::cerr << "vtkMEDReader::Reload" << a << std::endl;
  std::string fName((const char *)this->GetFileName());
  delete this->Internal;
  this->Internal=new vtkMEDReaderInternal(this);
  this->SetFileName(fName.c_str());
}

void vtkMEDReader::GenerateVectors(int val)
{
  if(this->Internal->FileName.empty())
    {//pvsm mode
      this->Internal->PK.pushGenerateVectorsValue(val);
      return ;
    }
  //not pvsm mode (general case)
  bool val2((bool)val);
  if(val2!=this->Internal->GenerateVect)
    {
      this->Internal->GenerateVect=val2;
      this->Modified();
    }
}

void vtkMEDReader::ChangeMode(int newMode)
{
  if(this->Internal->FileName.empty())
    {//pvsm mode
      this->Internal->PK.pushChangeModeValue(newMode);
      return ;
    }
  //not pvsm mode (general case)
  this->Internal->IsStdOrMode=newMode!=0;
  //std::cerr << "vtkMEDReader::ChangeMode : " << this->Internal->IsStdOrMode << std::endl;
  this->Modified();
}

const char *vtkMEDReader::GetSeparator()
{
  return MEDFileFieldRepresentationLeavesArrays::ZE_SEP;
}

void vtkMEDReader::SetFileName(const char *fname)
{
  try
    {
      this->Internal->FileName=fname;
      std::size_t pos(this->Internal->FileName.find_last_of('.'));
      if(pos!=std::string::npos)
        {
          std::string ext(this->Internal->FileName.substr(pos));
          if(ext.find("sauv")!=std::string::npos)
            this->Internal->IsMEDOrSauv=false;
        }
      if(this->Internal->Tree.getNumberOfLeavesArrays()==0)
        {
          this->Internal->Tree.loadMainStructureOfFile(this->Internal->FileName.c_str(),this->Internal->IsMEDOrSauv);
          if(!this->Internal->PK.arePropertiesOnTreeToSetAfter())
            this->Internal->Tree.activateTheFirst();//This line manually initialize the status of server (this) with the remote client.
          this->Internal->TK.setMaxNumberOfTimeSteps(this->Internal->Tree.getMaxNumberOfTimeSteps());
        }
      this->Modified();
      this->Internal->PK.assignPropertiesIfNeeded();
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      delete this->Internal;
      this->Internal=0;
      std::ostringstream oss;
      oss << "Exception has been thrown in vtkMEDReader::SetFileName : " << e.what() << std::endl;
      if(this->HasObserver("ErrorEvent") )
        this->InvokeEvent("ErrorEvent",const_cast<char *>(oss.str().c_str()));
      else
        vtkOutputWindowDisplayErrorText(const_cast<char *>(oss.str().c_str()));
      vtkObject::BreakOnError();
    }
}

char *vtkMEDReader::GetFileName()
{
  return const_cast<char *>(this->Internal->FileName.c_str());
}

int vtkMEDReader::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  //std::cerr << "########################################## RequestInformation ##########################################" << std::endl;
  if(!this->Internal)
    return 0;
  try
    {
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
      outInfo->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkMultiBlockDataSet");
      this->UpdateSIL(outInfo);
      //
      bool dummy(false);
      this->PublishTimeStepsIfNeeded(outInfo,dummy);
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkMEDReader::RequestInformation : " << e.what() << std::endl;
      return 0;
    }
  return 1;
}

int vtkMEDReader::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  //std::cerr << "########################################## RequestData        ##########################################";
  if(!this->Internal)
    return 0;
  try
    {
      vtkInformation *outInfo(outputVector->GetInformationObject(0));
      vtkMultiBlockDataSet *output(vtkMultiBlockDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())));
      bool isUpdated(false);
      double reqTS(0.);
      if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
        reqTS=outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
      //std::cerr << reqTS << std::endl;
      this->FillMultiBlockDataSetInstance(output,reqTS);
      output->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(),reqTS);
      this->UpdateSIL(outInfo);
      //this->UpdateProgress((float) progress/((float) maxprogress-1));
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      std::cerr << "Exception has been thrown in vtkMEDReader::RequestInformation : " << e.what() << std::endl;
      return 0;
    }
  return 1;
}

void vtkMEDReader::SetFieldsStatus(const char* name, int status)
{
  //std::cerr << "vtkMEDReader::SetFieldsStatus(" << name << "," << status << ") called !" << std::endl;
  if(this->Internal->FileName.empty())
    {//pvsm mode
      this->Internal->PK.pushFieldStatusEntry(name,status);
      return ;
    }
  //not pvsm mode (general case)
  try
    {
      this->Internal->Tree.changeStatusOfAndUpdateToHaveCoherentVTKDataSet(this->Internal->Tree.getIdHavingZeName(name),status);
      if(std::string(name)==GetFieldsTreeArrayName(GetNumberOfFieldsTreeArrays()-1))
        if(!this->Internal->PluginStart0())
          this->Modified();
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      if(!this->Internal->FileName.empty())
        {
          std::cerr << "vtkMEDReader::SetFieldsStatus error : " << e.what() << " *** WITH STATUS=" << status << endl;
          return ;
        }
    }
}

int vtkMEDReader::GetNumberOfFieldsTreeArrays()
{
  if(!this->Internal)
    return 0;
  int ret(this->Internal->Tree.getNumberOfLeavesArrays());
  //std::cerr << "vtkMEDReader::GetNumberOfFieldsTreeArrays called ! " << ret << std::endl;
  return ret;
}

const char *vtkMEDReader::GetFieldsTreeArrayName(int index)
{
  std::string ret(this->Internal->Tree.getNameOf(index));
  //std::cerr << "vtkMEDReader::GetFieldsTreeArrayName(" << index << ") called ! " << ret << std::endl;
  return ret.c_str();
}

int vtkMEDReader::GetFieldsTreeArrayStatus(const char *name)
{
  int zeId(this->Internal->Tree.getIdHavingZeName(name));
  int ret(this->Internal->Tree.getStatusOf(zeId));
  return ret;
}

void vtkMEDReader::SetTimesFlagsStatus(const char *name, int status)
{
  if(this->Internal->FileName.empty())
    {//pvsm mode
      this->Internal->PK.pushTimesFlagsStatusEntry(name,status);
      return ;
    }
  //not pvsm mode (general case)
  int pos(0);
  std::istringstream iss(name); iss >> pos;
  this->Internal->TK.getTimesFlagArray()[pos].first=(bool)status;
  if(pos==this->Internal->TK.getTimesFlagArray().size()-1)
    if(!this->Internal->PluginStart0())
      {
        this->Modified();
        //this->Internal->TK.printSelf(std::cerr);
      }
}

int vtkMEDReader::GetNumberOfTimesFlagsArrays()
{
  if(!this->Internal)
    return 0;
  return (int)this->Internal->TK.getTimesFlagArray().size();
}

const char *vtkMEDReader::GetTimesFlagsArrayName(int index)
{
  return this->Internal->TK.getTimesFlagArray()[index].second.c_str();
}

int vtkMEDReader::GetTimesFlagsArrayStatus(const char *name)
{
  int pos(0);
  std::istringstream iss(name); iss >> pos;
  return (int)this->Internal->TK.getTimesFlagArray()[pos].first;
}

void vtkMEDReader::UpdateSIL(vtkInformation *info)
{
  if(!this->Internal)
      return ;
  vtkMutableDirectedGraph *sil(vtkMutableDirectedGraph::New());
  std::string meshName(this->BuildSIL(sil));
  if(meshName!=this->Internal->DftMeshName)
    {
      if(this->Internal->SIL)
        this->Internal->SIL->Delete();
      this->Internal->SIL=sil;
      this->Internal->DftMeshName=meshName;
      info->Set(vtkDataObject::SIL(),this->Internal->SIL);
      //request->AppendUnique(vtkExecutive::KEYS_TO_COPY(),vtkDataObject::SIL());
    }
  else
    {
      sil->Delete();
    }
}

/*!
 * The returned string is the name of the mesh activated which groups and families are in \a sil.
 */
std::string vtkMEDReader::BuildSIL(vtkMutableDirectedGraph* sil)
{
  sil->Initialize();
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
  // Now build the hierarchy.
  vtkIdType rootId=sil->AddVertex();
  names.push_back("SIL");
  // Add global fields root
  vtkIdType fieldsRoot(sil->AddChild(rootId,childEdge));
  names.push_back("FieldsStatusTree");
  this->Internal->Tree.feedSIL(sil,fieldsRoot,childEdge,names);
  vtkIdType meshesFamsGrpsRoot(sil->AddChild(rootId,childEdge));
  names.push_back("MeshesFamsGrps");
  std::string dftMeshName(this->Internal->Tree.feedSILForFamsAndGrps(sil,meshesFamsGrpsRoot,childEdge,names));
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
  return dftMeshName;
}

double vtkMEDReader::PublishTimeStepsIfNeeded(vtkInformation *outInfo, bool& isUpdated)
{
  int lev0(-1);
  std::vector<double> tsteps;
  if(!this->Internal->IsStdOrMode)
    tsteps=this->Internal->Tree.getTimeSteps(lev0,this->Internal->TK);
  else
    { tsteps.resize(1); tsteps[0]=0.; }
  isUpdated=false;
  if(lev0!=this->Internal->LastLev0)
    {
      isUpdated=true;
      double timeRange[2];
      timeRange[0]=tsteps.front();
      timeRange[1]=tsteps.back();
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),&tsteps[0],tsteps.size());
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(),timeRange,2);
      this->Internal->LastLev0=lev0;
    }
  return tsteps.front();
}

void vtkMEDReader::FillMultiBlockDataSetInstance(vtkMultiBlockDataSet *output, double reqTS)
{
  std::string meshName;
  vtkDataSet *ret(this->Internal->Tree.buildVTKInstance(this->Internal->IsStdOrMode,reqTS,meshName,this->Internal->TK));
  if(this->Internal->GenerateVect)
    {
      vtkGenerateVectors::Operate(ret->GetPointData());
      vtkGenerateVectors::Operate(ret->GetCellData());
      vtkGenerateVectors::Operate(ret->GetFieldData());
    }
  output->SetBlock(0,ret);
  ret->Delete();
}

void vtkMEDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
