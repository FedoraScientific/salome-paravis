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

#include "vtkParaMEDCorbaSource.h"

#include "vtkPoints.h"
#include "vtkIntArray.h"
#include "vtkCellData.h"
#include "vtkCellTypes.h"
#include "vtkCharArray.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkUnstructuredGrid.h"
//
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkGenericAttributeCollection.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
//
#include "vtksys/stl/string"
#include "vtksys/ios/fstream"
#include "vtksys/stl/algorithm"

#include "VTKMEDCouplingMeshClient.hxx"
#include "VTKMEDCouplingFieldClient.hxx"
#include "VTKMEDCouplingMultiFieldsClient.hxx"
#include "VTKParaMEDFieldClient.hxx"

//Work with IOR.
#include "ParaMEDCouplingCorbaServant.hh"
//

vtkStandardNewMacro(vtkParaMEDCorbaSource);
//vtkCxxRevisionMacro(vtkParaMEDCorbaSource,"$Revision$");

void *vtkParaMEDCorbaSource::Orb=0;

vtkParaMEDCorbaSource::vtkParaMEDCorbaSource():mfieldsFetcher(0)
{
  this->MyDataSet=0;
  if(!Orb)
  {
    CORBA::ORB_var *OrbC=new CORBA::ORB_var;
    int argc=0;
    *OrbC=CORBA::ORB_init(argc,0);
    this->Orb=OrbC;
  }
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

vtkParaMEDCorbaSource::~vtkParaMEDCorbaSource()
{
  delete mfieldsFetcher;
}

const char *vtkParaMEDCorbaSource::GetIORCorba()
{
  return &IOR[0];
}

void vtkParaMEDCorbaSource::SetIORCorba(char *ior)
{
  if(!ior)
    return;
  if(ior[0]=='\0')
    return;
  int length=strlen(ior);
  IOR.resize(length+1);
  vtksys_stl::copy(ior,ior+length+1,&IOR[0]);
  this->Modified();
}

void vtkParaMEDCorbaSource::SetBufferingPolicy(int pol)
{
  BufferingPolicy=pol;
}

int vtkParaMEDCorbaSource::GetBufferingPolicy()
{
  return BufferingPolicy;
}

//int vtkParaMEDCorbaSource::RequestUpdateExtent( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo )
//{
//return this->Superclass::RequestUpdateExtent(request,inInfo,outInfo);

/*vtkParaMEDCorbaDataSet* output = vtkParaMEDCorbaDataSet::SafeDownCast( info->Get( vtkDataObject::DATA_OBJECT() ) );
  if ( ! output )
    {
    output = vtkParaMEDCorbaDataSet::New();
    output->SetPipelineInformation( info );
    output->Delete();
    this->GetOutputPortInformation( 0 )->Set( vtkDataObject::DATA_EXTENT_TYPE(), output->GetExtentType() );
    }*/

// return 1;
//}

int vtkParaMEDCorbaSource::ProcessRequest(vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
  {
    return this->RequestData(request, inputVector, outputVector);
  }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
  {
    return this->RequestInformation(request, inputVector, outputVector);
  }
  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

int vtkParaMEDCorbaSource::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet");
  return 1;
}

int vtkParaMEDCorbaSource::RequestInformation(vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo)
{
  vtkInformation* myInfo=outInfo->GetInformationObject(0);
  //myInfo->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkUnstructuredGrid");
  if(!IOR.empty())
  {
    //myInfo->Remove(vtkDataObject::DATA_TYPE_NAME());
    //myInfo->Remove(PORT_REQUIREMENTS_FILLED());
    //myInfo->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkUnstructuredGrid");
    //myInfo->Set(PORT_REQUIREMENTS_FILLED(),1);
    //vtkUnstructuredGrid *tony=vtkUnstructuredGrid::New();
    //tony->SetInformation(myInfo);
    //myInfo->Set(vtkDataObject::DATA_OBJECT(),tony);
    //
    CORBA::ORB_var *OrbC=(CORBA::ORB_var *)this->Orb;
    CORBA::Object_var obj=(*OrbC)->string_to_object(&IOR[0]);
    //
    Engines::MPIObject_ptr objPara=Engines::MPIObject::_narrow(obj);
    if(CORBA::is_nil(objPara))
    {//sequential
      this->TotalNumberOfPieces=1;
      SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_var multiPtr=SALOME_MED::MEDCouplingMultiFieldsCorbaInterface::_narrow(obj);
      if(!CORBA::is_nil(multiPtr))
      {//Request for multiFields
        delete mfieldsFetcher;
        mfieldsFetcher=new ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher(BufferingPolicy,multiPtr);
        std::vector<double> tsteps=mfieldsFetcher->getTimeStepsForPV();
        double timeRange[2];
        timeRange[0]=tsteps.front();
        timeRange[1]=tsteps.back();
        myInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),&tsteps[0],tsteps.size());
        myInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(),timeRange,2);
      }
    }
    else
    {
      Engines::IORTab *iorTab=objPara->tior();
      this->TotalNumberOfPieces=iorTab->length();
      delete iorTab;
      CORBA::release(objPara);
    }
    myInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),this->TotalNumberOfPieces);
  }
  return 1;
}

int vtkParaMEDCorbaSource::RequestData(vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outputVector)
{
  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  //
  this->UpdatePiece = vtkStreamingDemandDrivenPipeline::GetUpdatePiece(outInfo);
  this->NumberOfPieces = vtkStreamingDemandDrivenPipeline::GetUpdateNumberOfPieces(outInfo);
  this->GhostLevel = vtkStreamingDemandDrivenPipeline::GetUpdateGhostLevel(outInfo);
  this->StartPiece=((this->UpdatePiece*this->TotalNumberOfPieces)/this->NumberOfPieces);
  this->EndPiece=(((this->UpdatePiece+1)*this->TotalNumberOfPieces)/this->NumberOfPieces);
  vtkMultiBlockDataSet *ret0=vtkMultiBlockDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  double reqTS = 0;
  if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    reqTS = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
  //Client request on ORB.
  CORBA::ORB_var *OrbC=(CORBA::ORB_var *)this->Orb;
  CORBA::Object_var obj=(*OrbC)->string_to_object(&IOR[0]);
  //
  Engines::MPIObject_var objPara=Engines::MPIObject::_narrow(obj);
  if(CORBA::is_nil(objPara))
  {//sequential
    SALOME_MED::MEDCouplingMeshCorbaInterface_var meshPtr=SALOME_MED::MEDCouplingMeshCorbaInterface::_narrow(obj);
    if(!CORBA::is_nil(meshPtr))
    {
      bool dummy;//bug VTK
      vtkDataSet *ret=ParaMEDMEM2VTK::BuildFromMEDCouplingMeshInstance(meshPtr,dummy);//bug VTK
      if(!ret)
        return 0;
      ret0->SetBlock(0,ret);
      ret->Delete();
      return 1;
    }
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(obj);
    if(!CORBA::is_nil(fieldPtr))
    {
      std::vector<double> ret2;
      vtkDataSet *ret=ParaMEDMEM2VTK::BuildFullyFilledFromMEDCouplingFieldDoubleInstance(fieldPtr,ret2);
      if(!ret)
      {
        vtkErrorMacro("On single field CORBA fetching an error occurs !");
        return 0;
      }
      ret0->SetBlock(0,ret);
      ret->Delete();
      //
      double timeRange[2];
      timeRange[0]=ret2[0];
      timeRange[1]=ret2[0];
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),&ret2[0],1);
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(),timeRange,2);
      ret0->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(),ret2[0]);
      return 1;
    }
    SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_var multiPtr=SALOME_MED::MEDCouplingMultiFieldsCorbaInterface::_narrow(obj);
    if(!CORBA::is_nil(multiPtr))
    {
      vtkDataSet *ret=mfieldsFetcher->buildDataSetOnTime(reqTS);
      if(!ret)
      {
        vtkErrorMacro("On multi fields CORBA fetching an error occurs !");
        return 0;
      }
      ret0->SetBlock(0,ret);
      ret->Delete();
      ret0->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(),reqTS);
      return 1;
    }
    vtkErrorMacro("Unrecognized sequential CORBA reference !");
    return 0;
  }
  else
  {
    SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface_var paraFieldCorba=SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface::_narrow(obj);
    if(!CORBA::is_nil(paraFieldCorba))
    {
      ParaMEDMEM2VTK::FillMEDCouplingParaFieldDoubleInstanceFrom(paraFieldCorba,this->StartPiece,this->EndPiece,ret0);
      return 1;
    }
    vtkErrorMacro("Unrecognized parallel CORBA reference !");
    return 0;
  }
}

void vtkParaMEDCorbaSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf( os, indent );
  os << "Data: " << this->MyDataSet << "\n";
}

