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

#include "PARAVIS_Engine_i.hh"
#include "utilities.h"


#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include "SALOME_NamingService.hxx"
#include "Utils_ExceptHandlers.hxx"

using namespace std;

#ifdef _DEBUG_
static int MYDEBUG = 1;
#else
static int MYDEBUG = 0;
#endif

#ifdef WNT
#if defined PARAVIS_ENGINE_EXPORTS || defined PARAVISEngine_EXPORTS
#define PARAVIS_ENGINE_EXPORT __declspec(dllexport)
#else
#define PARAVIS_ENGINE_EXPORT __declspec(dllimport)
#endif
#else
#define PARAVIS_ENGINE_EXPORT
#endif

UNEXPECT_CATCH(SalomeException, SALOME::SALOME_Exception);

extern "C" {
PARAVIS_ENGINE_EXPORT PortableServer::ObjectId* PARAVISEngine_factory(CORBA::ORB_ptr orb, 
                                                                      PortableServer::POA_ptr poa, 
                                                                      PortableServer::ObjectId * contId,
                                                                      const char *instanceName, 
                                                                      const char *interfaceName) 
{
  MESSAGE("PARAVISEngine_factory : "<<interfaceName);
  SALOME_NamingService aNamingService(orb);
  CORBA::Object_ptr anObject = aNamingService.Resolve("/Kernel/Session");
  SALOME::Session_var aSession = SALOME::Session::_narrow(anObject);
  if (CORBA::is_nil(aSession))
    return NULL;
  
  PARAVIS::PARAVIS_Gen_i* pPARAVIS_Gen = new PARAVIS::PARAVIS_Gen_i(aSession, 
                                                                    orb, 
                                                                    poa, 
                                                                    contId, 
                                                                    instanceName, 
                                                                    interfaceName);
  return pPARAVIS_Gen->getId() ;
}
}

namespace PARAVIS {
  //===========================================================================
  PARAVIS_Gen_i::PARAVIS_Gen_i(SALOME::Session_ptr session,
                               CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               PortableServer::ObjectId * contId, 
                               const char *instanceName, 
                               const char *interfaceName) :
    Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
  {
    _thisObj = this ;
    _id = _poa->activate_object(_thisObj);
#ifndef WIN32
    Engines::EngineComponent_var aComponent = session->GetComponent("libPARAVIS.so");
#else
    Engines::EngineComponent_var aComponent = session->GetComponent("PARAVIS.dll");
#endif
    if (CORBA::is_nil(aComponent)) {
      MESSAGE("Component PARAVIS is null");
      return;
    }
    myParaVisGen = PARAVIS::PARAVIS_Gen::_narrow(aComponent);
  } 
  
  //===========================================================================
  PARAVIS_Gen_i::~PARAVIS_Gen_i()
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::~PARAVIS_Gen_i");
  }


  //===========================================================================
  char* PARAVIS_Gen_i::GetIOR(){
    return myParaVisGen->GetIOR();
  }

  //===========================================================================
  bool PARAVIS_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent,
                        const SALOMEDS::TMPFile & theStream,
                        const char* theURL,
                        bool isMultiFile)
  {
    return myParaVisGen->Load(theComponent,theStream,theURL,isMultiFile);
  }

  //===========================================================================
  bool PARAVIS_Gen_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent,
                             const SALOMEDS::TMPFile & theStream,
                             const char* theURL,
                             bool isMultiFile) 
  {
    return Load(theComponent, theStream, theURL, isMultiFile);
  }


  //===========================================================================
  char* PARAVIS_Gen_i::LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
                                           const char* aLocalPersistentID,
                                           CORBA::Boolean isMultiFile,
                                           CORBA::Boolean isASCII) 
  {
    return myParaVisGen->LocalPersistentIDToIOR(theSObject, aLocalPersistentID, isMultiFile, isASCII);
  }

  //===========================================================================
  SALOMEDS::TMPFile* PARAVIS_Gen_i::Save(SALOMEDS::SComponent_ptr theComponent,
				      const char* theURL,
				      bool isMultiFile)
  {
    return myParaVisGen->Save(theComponent,theURL,isMultiFile);
  }

  //===========================================================================
  SALOMEDS::TMPFile* PARAVIS_Gen_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
					   const char* theURL,
					   bool isMultiFile) 
  {
    return myParaVisGen->Save(theComponent,theURL,isMultiFile);
  }


  //===========================================================================
  char* PARAVIS_Gen_i::IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
					   const char* IORString,
					   CORBA::Boolean isMultiFile,
					   CORBA::Boolean isASCII) 
  {
    return myParaVisGen->IORToLocalPersistentID(theSObject, IORString, isMultiFile, isASCII);
  }

  //===========================================================================
  void PARAVIS_Gen_i::ImportFile(const char* theFileName)
  {
    if (CORBA::is_nil(myParaVisGen)) {
      MESSAGE("PARAVIS_Gen_i is null");
      return;
    }
    myParaVisGen->ImportFile(theFileName);
  }

  void PARAVIS_Gen_i::ExecuteScript(const char* script)
  {
    if (CORBA::is_nil(myParaVisGen)) {
      MESSAGE("PARAVIS_Gen_i is null");
      return;
    }
    myParaVisGen->ExecuteScript(script);
  }

  //===========================================================================
  char* PARAVIS_Gen_i::GetTrace()
  {
    if (CORBA::is_nil(myParaVisGen)) {
      MESSAGE("PARAVIS_Gen_i is null");
      return CORBA::string_dup("");
    }
    return CORBA::string_dup(myParaVisGen->GetTrace());
  }

  //===========================================================================
  void PARAVIS_Gen_i::SaveTrace(const char* theFileName)
  {
    if (CORBA::is_nil(myParaVisGen)) {
      MESSAGE("PARAVIS_Gen_i is null");
      return ;
    }
    myParaVisGen->SaveTrace(theFileName);
  }

  //===========================================================================
  void PARAVIS_Gen_i::Close(SALOMEDS::SComponent_ptr theComponent)
  {
    myParaVisGen->Close(theComponent);
  }


  //===========================================================================
  char* PARAVIS_Gen_i::ComponentDataType()
  {
    return CORBA::string_dup("PARAVIS");
  }

  //===========================================================================
  CORBA::Boolean PARAVIS_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR) 
  {
    return myParaVisGen->CanPublishInStudy(theIOR);
  }


  //===========================================================================
  SALOMEDS::SObject_ptr PARAVIS_Gen_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
						   SALOMEDS::SObject_ptr theSObject,
						   CORBA::Object_ptr theObject,
						   const char* theName) 
    throw (SALOME::SALOME_Exception) 
  {
    return myParaVisGen->PublishInStudy(theStudy, theSObject, theObject, theName);
  }

  //===========================================================================
  CORBA::Boolean PARAVIS_Gen_i::CanCopy(SALOMEDS::SObject_ptr theObject) 
  {
    return myParaVisGen->CanCopy(theObject);
  }
  

  //===========================================================================
  SALOMEDS::TMPFile* PARAVIS_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID) 
  {
    return myParaVisGen->CopyFrom(theObject, theObjectID);
  }
  

  //===========================================================================
  CORBA::Boolean PARAVIS_Gen_i::CanPaste(const char* theComponentName, CORBA::Long theObjectID) 
  {
    return myParaVisGen->CanPaste(theComponentName, theObjectID);
  }

  //===========================================================================
  SALOMEDS::SObject_ptr PARAVIS_Gen_i::PasteInto(const SALOMEDS::TMPFile& theStream,
					      CORBA::Long theObjectID,
					      SALOMEDS::SObject_ptr theObject) 
  {
    return myParaVisGen->PasteInto(theStream,theObjectID,theObject);
  }

  //===========================================================================
  PARAVIS::string_array* PARAVIS_Gen_i::GetClassesList()
  {
    return myParaVisGen->GetClassesList();
  }

  //===========================================================================
  PARAVIS_Base_ptr PARAVIS_Gen_i::CreateClass(const char* theClassName)
  {
    return myParaVisGen->CreateClass(theClassName);
  }

  //===========================================================================
  void PARAVIS_Gen_i::GetConnectionParameters(CORBA::Long& theId, 
                                              CORBA::String_out theDHost, CORBA::Long& theDPort,
                                              CORBA::String_out theRHost, CORBA::Long& theRPort,
                                              CORBA::Long& theReversed)
  {
    myParaVisGen->GetConnectionParameters(theId, theDHost, theDPort, theRHost, theRPort, theReversed);
  }
    
  //===========================================================================
  void PARAVIS_Gen_i::ActivateModule()
  {
    myParaVisGen->ActivateModule();
  }

  //===========================================================================
  void PARAVIS_Gen_i::SetCurrentStudy(SALOMEDS::Study_ptr theStudy){
    myParaVisGen->SetCurrentStudy(theStudy);
  }


  //===========================================================================
  SALOMEDS::Study_ptr PARAVIS_Gen_i::GetCurrentStudy(){
    return myParaVisGen->GetCurrentStudy();
  }
}
