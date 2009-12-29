//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  PARAVIS OBJECT : interactive object for PARAVIS entities implementation
//  File   : PARAVIS_Gen_i.cc
//  Author : Vitaly Smetannikov
//  Module : PARAVIS
//


#include "PARAVIS_Gen_i.hh"

// IDL Headers
#include <omnithread.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include <CAM_Module.h>
#include "PVGUI_Module.h"
#include <SalomeApp_Application.h>

#include <SALOMEDS_Tool.hxx>
#include <QFileInfo>

#include "PV_Events.h"
#include "PV_Tools.h"

#include "QDomDocument"
#include "QDomNode"
#include "QDomElement"
#include "QFile"
#include "QTextStream"

#include "vtkWrapIDL.h"

#include <pqServer.h>
#include <pqServerResource.h>


#ifdef _DEBUG_
static int MYDEBUG = 1;
#else
static int MYDEBUG = 0;
#endif

using namespace std;

extern PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString&);

extern "C" 
PARAVIS_I_EXPORT PARAVIS::PARAVIS_Gen_ptr GetImpl(CORBA::ORB_ptr theORB, 
                                                  PortableServer::POA_ptr thePOA,
                                                  SALOME_NamingService* theNamingService,
                                                  QMutex* theMutex)
{
  if(MYDEBUG) MESSAGE("extern 'C' GetImpl");
  PARAVIS::PARAVIS_Gen_i *aPARAVIS_Gen = new PARAVIS::PARAVIS_Gen_i(theORB, 
                                                                    thePOA, 
                                                                    theNamingService, 
                                                                    theMutex);
  return aPARAVIS_Gen->_this();
}



namespace PARAVIS
{
  PARAVIS_Base_i::~PARAVIS_Base_i() {
    if(mySmartPointer != NULL) mySmartPointer->Delete();
  }

  void PARAVIS_Base_i::Init(::vtkObjectBase* base) {
    if (mySmartPointer != NULL) mySmartPointer->Delete();
    mySmartPointer = base;
  }

  ::vtkObjectBase* PARAVIS_Base_i::getVTKObject(PARAVIS_Base_ptr theBase) {
    PARAVIS_Base_i* aBase = GET_SERVANT(theBase);
    return (aBase != NULL)? aBase->getVTKObject() : NULL;
  }

  CORBA::Boolean PARAVIS_Base_i::IsSame(PARAVIS_Base_ptr theOther)
  {
    PARAVIS_Base_i* aBase = GET_SERVANT(theOther);
    if (aBase == NULL) 
      return false;
    return mySmartPointer.GetPointer() == aBase->getVTKObject();
  }




  PARAVIS_Gen_i*          PARAVIS_Gen_i::myParavisGenImpl;
  CORBA::ORB_var          PARAVIS_Gen_i::myOrb;
  PortableServer::POA_var PARAVIS_Gen_i::myPoa;
  SALOME_LifeCycleCORBA*  PARAVIS_Gen_i::myEnginesLifeCycle;
  SALOME_NamingService*   PARAVIS_Gen_i::myNamingService;
  QMutex*                 PARAVIS_Gen_i::myMutex;
  SalomeApp_Application*  PARAVIS_Gen_i::mySalomeApp = 0;


  //----------------------------------------------------------------------------
  PARAVIS_Gen_i::PARAVIS_Gen_i(CORBA::ORB_ptr theORB, PortableServer::POA_ptr thePOA,
                               SALOME_NamingService* theNamingService, QMutex* theMutex) :
    Engines_Component_i()
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::PARAVIS_Gen_i");
    if (mySalomeApp) return;

    myMutex = theMutex;
    myOrb = CORBA::ORB::_duplicate(theORB);
    myPoa = PortableServer::POA::_duplicate(thePOA);
    myParavisGenImpl = this;
    myNamingService = theNamingService;

    static SALOME_LifeCycleCORBA aEnginesLifeCycle(theNamingService);
    myEnginesLifeCycle = &aEnginesLifeCycle;

    CORBA::Object_var anObj = myNamingService->Resolve("/myStudyManager");
    if (!CORBA::is_nil(anObj)) {
      SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(anObj);
      SALOMEDS::ListOfOpenStudies_var aListOfOpenStudies = aStudyManager->GetOpenStudies();
      if(aListOfOpenStudies->length() > 0) {
        CORBA::String_var aStudyName = aListOfOpenStudies[0];
        myStudyDocument = aStudyManager->GetStudyByName(aStudyName);
        if (!myStudyDocument->_is_nil()) {
          mySalomeApp = ProcessEvent(new TGetGUIApplication(myStudyDocument->StudyId()));
          if(!myStudyDocument->GetProperties()->IsLocked())
            FindOrCreateParaVisComponent(myStudyDocument);
        }
      } else
        if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::PARAVIS_Gen_i : there is no opened study in StudyManager !!!");
    } else
      if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::PARAVIS_Gen_i : Can't find StudyManager !!!");

  }

  //----------------------------------------------------------------------------
  PARAVIS_Gen_i::~PARAVIS_Gen_i()
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::~PARAVIS_Gen_i");
  }

  //----------------------------------------------------------------------------
  char* PARAVIS_Gen_i::GetIOR()
  {
    if(myIOR == ""){
      CORBA::Object_var anObject = _this();
      CORBA::String_var anIOR = myOrb->object_to_string(anObject);
      myIOR = anIOR.in();
    }
    return CORBA::string_dup(myIOR.c_str());
  }

  //----------------------------------------------------------------------------
  void PARAVIS_Gen_i::ImportFile(const char* theFileName)
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::ImportFile: " <<theFileName);
    ProcessVoidEvent(new TImportFile(mySalomeApp, theFileName));
  }

  //----------------------------------------------------------------------------
  char* PARAVIS_Gen_i::GetTrace()
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::PrintTrace: ");
    return CORBA::string_dup(ProcessEvent(new TGetTrace(mySalomeApp)));
  }

  //----------------------------------------------------------------------------
  void PARAVIS_Gen_i::SaveTrace(const char* theFileName)
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::SaveTrace: " <<theFileName);
    ProcessVoidEvent(new TSaveTrace(mySalomeApp, theFileName));
  }

  //----------------------------------------------------------------------------
  void processElements(QDomNode& thePropertyNode, QStringList& theFileNames, const char* theNewPath)
  {
    QDomNode aElementNode = thePropertyNode.firstChild();
    while (aElementNode.isElement()) {
      QDomElement aElement = aElementNode.toElement();
      if (aElement.tagName() == "Element") {
        QString aIndex = aElement.attribute("index");
        if (aIndex == "0") {
          QString aValue = aElement.attribute("value");
          if (!aValue.isNull()) {
            if (theNewPath == 0) {
              QFileInfo aFInfo(aValue);
              theFileNames<<aValue;
              aElement.setAttribute("value", aFInfo.fileName());
              break;
            } else {
              aElement.setAttribute("value", QString(theNewPath) + aValue);            
            }
          }
        }
      }
      aElementNode = aElementNode.nextSibling();
    }
  }


  //----------------------------------------------------------------------------
  void processProperties(QDomNode& theProxyNode, QStringList& theFileNames, const char* theNewPath)
  {
    QDomNode aPropertyNode = theProxyNode.firstChild();
    while (aPropertyNode.isElement()) {
      QDomElement aProperty = aPropertyNode.toElement();
      QString aName = aProperty.attribute("name");
      if ((aName == "FileName") || (aName == "FileNameInfo") || (aName == "FileNames")) {
        processElements(aPropertyNode, theFileNames, theNewPath);
      }
      aPropertyNode = aPropertyNode.nextSibling();
    }
  }  


  //----------------------------------------------------------------------------
  void processProxies(QDomNode& theNode, QStringList& theFileNames, const char* theNewPath)
  {
    QDomNode aProxyNode = theNode.firstChild();
    while (aProxyNode.isElement()) {
      QDomElement aProxy = aProxyNode.toElement();
      if (aProxy.tagName() == "Proxy") {
        QString aGroup = aProxy.attribute("group");
        if (aGroup == "sources") {
          processProperties(aProxyNode, theFileNames, theNewPath);
        }
      }
      aProxyNode = aProxyNode.nextSibling();
    }
  }

  //----------------------------------------------------------------------------
  bool processAllFilesInState(const char* aFileName, QStringList& theFileNames, const char* theNewPath)
  {
    QFile aFile(aFileName);
    if (!aFile.open(QFile::ReadOnly)) {
      MESSAGE("Can't open state file "<<aFileName);
      return false;
    }
    QDomDocument aDoc;
    bool aRes = aDoc.setContent(&aFile);
    aFile.close();
    
    if (!aRes) {
      MESSAGE("File "<<aFileName<<" is not XML document");
      return false;
    }
    
    QDomElement aRoot = aDoc.documentElement();
    if ( aRoot.isNull() || aRoot.tagName() != "ParaView" ) {
      MESSAGE( "Invalid XML root" );
      return false;
    }

    QDomNode aNode = aRoot.firstChild();
    while (aRes  && !aNode.isNull() ) {
      aRes = aNode.isElement();
      if ( aRes ) {
        QDomElement aSection = aNode.toElement();
        if (aSection.tagName() == "ServerManagerState") {
          processProxies(aNode, theFileNames, theNewPath);
        }
      }
      aNode = aNode.nextSibling();
    }
    if (!aFile.open(QFile::WriteOnly | QFile::Truncate)) {
      MESSAGE("Can't open state file "<<aFileName<<" for writing");
      return false;
    }
    QTextStream out(&aFile);
    aDoc.save(out, 2);
    aFile.close();
    
    return true;
  }


  //----------------------------------------------------------------------------
  SALOMEDS::TMPFile* PARAVIS_Gen_i::Save(SALOMEDS::SComponent_ptr theComponent, 
                                         const char* theURL, bool isMultiFile)
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::Save - theURL = '"<<theURL<<"'");

    std::string aTmpDir = SALOMEDS_Tool::GetTmpDir();

    std::ostringstream aStream;
    aStream<<"paravisstate:"<<(long)this;
    std::string aFileName = "_" + aStream.str();
    if(MYDEBUG) MESSAGE("aFileName = '"<<aFileName);

    //std::vector<std::string> aFileNames;
    //aFileNames.push_back(aFileName);

    SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile(0);

    std::string aFile = aTmpDir + aFileName;
    ProcessVoidEvent(new TSaveStateFile(mySalomeApp, aFile.c_str()));

    // TO Do: Collect all files from state
    QStringList aFileNames;
    processAllFilesInState(aFile.c_str(), aFileNames, 0);
    aFileNames<<QString(aFile.c_str());

    QStringList aNames;
    foreach(QString aFile, aFileNames) {
      QFileInfo aInfo(aFile);
      aNames<<aInfo.fileName();
    }
    
    SALOMEDS::ListOfFileNames_var aListOfFileNames = GetListOfFileNames(aFileNames);
    SALOMEDS::ListOfFileNames_var aListOfNames = GetListOfFileNames(aNames);

    aStreamFile = SALOMEDS_Tool::PutFilesToStream(aListOfFileNames.in(), aListOfNames.in());
    SALOMEDS_Tool::RemoveTemporaryFiles(theURL, aListOfFileNames, true);

    return aStreamFile._retn();
  }
    
  //----------------------------------------------------------------------------
  SALOMEDS::TMPFile* PARAVIS_Gen_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                                              const char* theURL, bool isMultiFile)
  {
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::Save - theURL = '"<<theURL<<"'");

    std::string aTmpDir = SALOMEDS_Tool::GetTmpDir();

    std::ostringstream aStream;
    aStream<<"paravisstate:"<<(long)this;
    std::string aFileName = "_" + aStream.str();
    if(MYDEBUG) MESSAGE("aFileName = '"<<aFileName);

    SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile(0);

    std::string aFile = aTmpDir + aFileName;
    ProcessVoidEvent(new TSaveStateFile(mySalomeApp, aFile.c_str()));

    //Collect all files from state
    QStringList aFileNames;
    processAllFilesInState(aFile.c_str(), aFileNames, 0);
    aFileNames<<QString(aFile.c_str());

    QStringList aNames;
    foreach(QString aFile, aFileNames) {
      QFileInfo aInfo(aFile);
      aNames<<aInfo.fileName();
    }
    
    SALOMEDS::ListOfFileNames_var aListOfFileNames = GetListOfFileNames(aFileNames);
    SALOMEDS::ListOfFileNames_var aListOfNames = GetListOfFileNames(aNames);

    aStreamFile = SALOMEDS_Tool::PutFilesToStream(aListOfFileNames.in(), aListOfNames.in());
    SALOMEDS_Tool::RemoveTemporaryFiles(theURL, aListOfFileNames, true);

    return aStreamFile._retn();
  }
    

  //----------------------------------------------------------------------------
  bool PARAVIS_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent,	 const SALOMEDS::TMPFile& theStream,
                           const char* theURL, bool isMultiFile)
  {
    std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::Load - "<<aTmpDir);

    SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir, false);
    if (aSeq->length() == 0)
      return false;

    std::string aFile = aTmpDir + std::string(aSeq[aSeq->length() - 1]);
    QStringList aEmptyList;
    processAllFilesInState(aFile.c_str(), aEmptyList, aTmpDir.c_str());
    ProcessVoidEvent(new TLoadStateFile(mySalomeApp, aFile.c_str()));

    return true;
  }

  //----------------------------------------------------------------------------
  bool PARAVIS_Gen_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent, 
                                const SALOMEDS::TMPFile& theStream,
                                const char* theURL, bool isMultiFile)
  {
    std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();
    if(MYDEBUG) MESSAGE("PARAVIS_Gen_i::Load - "<<aTmpDir);

    SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir, false);
    if (aSeq->length() == 0)
      return false;

    std::string aFile = aTmpDir + std::string(aSeq[aSeq->length() - 1]);
    QStringList aEmptyList;
    processAllFilesInState(aFile.c_str(), aEmptyList, aTmpDir.c_str());
    ProcessVoidEvent(new TLoadStateFile(mySalomeApp, aFile.c_str()));

    return true;
  }
    
  //----------------------------------------------------------------------------
  void PARAVIS_Gen_i::Close(SALOMEDS::SComponent_ptr IORSComponent)
  {
  }
  
  //----------------------------------------------------------------------------
  char* PARAVIS_Gen_i::ComponentDataType()
  {
    return CORBA::string_dup("PARAVIS");
  }

  //----------------------------------------------------------------------------
  char* PARAVIS_Gen_i::IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject, const char* IORString,
                                              CORBA::Boolean isMultiFile, CORBA::Boolean isASCII)
  {
    CORBA::String_var aString("");
    return aString._retn();
  }

  //----------------------------------------------------------------------------
  char* PARAVIS_Gen_i::LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject, 
                                              const char* aLocalPersistentID,
                                              CORBA::Boolean isMultiFile, 
                                              CORBA::Boolean isASCII)
  {
    CORBA::String_var aString("");
    return aString._retn();
  }

  //----------------------------------------------------------------------------
  bool PARAVIS_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
  {
    return false;
  }

  //----------------------------------------------------------------------------
  SALOMEDS::SObject_ptr PARAVIS_Gen_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                                      SALOMEDS::SObject_ptr theSObject, 
                                                      CORBA::Object_ptr theObject,
                                                      const char* theName) 
    throw (SALOME::SALOME_Exception)
  {
    SALOMEDS::SObject_var aResultSO;
    return aResultSO._retn();
  }

  //----------------------------------------------------------------------------
  CORBA::Boolean PARAVIS_Gen_i::CanCopy(SALOMEDS::SObject_ptr theObject)
  {
    return false;
  }

  //----------------------------------------------------------------------------
  SALOMEDS::TMPFile* PARAVIS_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID)
  {
    SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;
    return aStreamFile._retn();
  }

  //----------------------------------------------------------------------------
  CORBA::Boolean PARAVIS_Gen_i::CanPaste(const char* theComponentName, CORBA::Long theObjectID)
  {
    return false;
  }

  //----------------------------------------------------------------------------
  SALOMEDS::SObject_ptr PARAVIS_Gen_i::PasteInto(const SALOMEDS::TMPFile& theStream,
                                                 CORBA::Long theObjectID, SALOMEDS::SObject_ptr theObject)
  {
    return SALOMEDS::SObject::_nil();
  }

  //----------------------------------------------------------------------------
  PARAVIS::string_array* PARAVIS_Gen_i::GetClassesList()
  {
    int k;
    for (k = 0; strcmp(wrapped_classes[k], "") != 0; k++);
    PARAVIS::string_array_var aNames = new PARAVIS::string_array();
    aNames->length(k);
    for (CORBA::ULong i = 0; i < k; i++)
      aNames[i] = CORBA::string_dup(wrapped_classes[i]);
    return aNames._retn();
  }

  //----------------------------------------------------------------------------
  PARAVIS_Base_ptr PARAVIS_Gen_i::CreateClass(const char* theClassName)
  {
    PARAVIS::PARAVIS_Base_i* aClass = CreateInstance(NULL, QString(theClassName));
    return aClass->_this();
  }

  //----------------------------------------------------------------------------
  void PARAVIS_Gen_i::GetConnectionParameters(CORBA::Long& theId, 
                                              CORBA::String_out theDHost, CORBA::Long& theDPort,
                                              CORBA::String_out theRHost, CORBA::Long& theRPort,
                                              CORBA::Long& theReversed)
  {
    pqServer* aServer = ProcessEvent(new TGetActiveServer(mySalomeApp));
    if (aServer) {
      theId = static_cast<int>(aServer->GetConnectionID());

      pqServerResource serverRes = aServer->getResource();
      theReversed = (serverRes.scheme() == "csrc" || serverRes.scheme() == "cdsrsrc")? 1 : 0;

      theDPort = serverRes.dataServerPort() < 0? serverRes.port() : serverRes.dataServerPort();
      theRPort = serverRes.renderServerPort() < 0 ? 0 : serverRes.renderServerPort();

      QString dsHost = serverRes.dataServerHost().isEmpty()? serverRes.host() : serverRes.dataServerHost();
      QString rsHost = serverRes.renderServerHost();

      theDHost = CORBA::string_dup(qPrintable(dsHost));
      theRHost = CORBA::string_dup(qPrintable(rsHost));
    }
  }


  //----------------------------------------------------------------------------
  Engines::TMPFile* PARAVIS_Gen_i::DumpPython(CORBA::Object_ptr theStudy,
                                              CORBA::Boolean theIsPublished,
                                              CORBA::Boolean& theIsValidScript)
  {
    theIsValidScript = true;
    std::string aResult(ProcessEvent(new TGetTrace(mySalomeApp)));
    aResult += "\ndef RebuildData(theStudy):\n  pass\n";
    CORBA::ULong aSize = aResult.size() + 1;
    char* aBuffer = new char[aSize];
    strcpy(aBuffer, &aResult[0]);
    Engines::TMPFile_var aDump = new Engines::TMPFile(aSize,aSize,(CORBA::Octet*)aBuffer,1);
    return aDump._retn();
  }

  //----------------------------------------------------------------------------
  void PARAVIS_Gen_i::ActivateModule()
  {
    ProcessVoidEvent(new TActivateModule(mySalomeApp));
  }
}
