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
//  File   : PARAVIS_Gen_i.hh
//  Author : Vitaly Smetannikov
//  Module : PARAVIS
//
#ifndef __PARAVIS_GEN_I_H__
#define __PARAVIS_GEN_I_H__

#include "PV_I.h"
#include "vtkSmartPointer.h"

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(PARAVIS_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"

#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

#include "vtkSmartPointer.h"

// QT Includes
#include <QMutex>

class SalomeApp_Application;
class PVGUI_Module;
class vtkObjectBase;

#define PVSTATEID 15555

namespace PARAVIS
{

  const char* checkNullStr(const char* theStr);

  /*!
   * Base implementation for all container objects for Paravis serevrmanager API classes
   */
  class PARAVIS_Base_i : public virtual POA_PARAVIS::PARAVIS_Base
  {
  public:
    //! Constructor
    PARAVIS_Base_i() {};

    //! Destructor
    virtual ~PARAVIS_Base_i();

    //! Initialises internal pointer on existing Paraview class instance
    virtual void Init(::vtkObjectBase* base);

    virtual ::vtkObjectBase* GetNew() { return NULL; }

    //! Returns pointer on internal Paraview class instance
    virtual ::vtkObjectBase* getVTKObject() 
    { if (mySmartPointer == NULL)
        mySmartPointer = GetNew();
      return mySmartPointer.GetPointer(); 
    }

    //! The same as previous in static context
    static ::vtkObjectBase* getVTKObject(PARAVIS_Base_ptr theBase);

    //! Compares two container objects comparing of their internal pointers
    virtual CORBA::Boolean IsSame(PARAVIS_Base_ptr theOther);

  protected:

    //! Pointer on wrapped class
    vtkSmartPointer< ::vtkObjectBase> mySmartPointer;
  };

  //----------------------------------------------------------------------------
  class PARAVIS_Gen_i : public virtual POA_PARAVIS::PARAVIS_Gen,
		     public virtual ::Engines_Component_i
  {
    //! Copy constructor
    PARAVIS_Gen_i(const PARAVIS::PARAVIS_Gen_i &);
  public:
    
    //! Constructor
    PARAVIS_Gen_i(CORBA::ORB_ptr theORB,
                  PortableServer::POA_ptr thePOA,
                  SALOME_NamingService* theNamingService, 
                  QMutex* theMutex);
    
    //! Destructor
    virtual ~PARAVIS_Gen_i();
    
    //! Returns CORBA ID for the current instance
    virtual char* GetIOR();

    //! Import file to PARAVIS module by its name. 
    virtual void ImportFile(const char* theFileName);

    //! Execute a PARAVIS script on Session
    virtual void ExecuteScript(const char* script);
    
    //! Returns Trace string. If Trace is not activated then empty string is returned
    virtual char* GetTrace();

    //! Save trace sting to a disk file.
    virtual void SaveTrace(const char* theFileName);

    //! inherited methods from Engines::Component
    virtual Engines::TMPFile* DumpPython(CORBA::Object_ptr theStudy,
                                         CORBA::Boolean theIsPublished,
					 CORBA::Boolean theIsMultiFile,
                                         CORBA::Boolean& theIsValidScript);

    //! inherited methods from SALOMEDS::Driver
    virtual SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent, 
                                    const char* theURL, bool isMultiFile);
    
    //! inherited methods from SALOMEDS::Driver
    virtual SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                                         const char* theURL, bool isMultiFile);
    
    //! inherited methods from SALOMEDS::Driver
    virtual bool Load(SALOMEDS::SComponent_ptr,	 const SALOMEDS::TMPFile &,
                      const char* theURL, bool isMultiFile);

    //! inherited methods from SALOMEDS::Driver
    virtual bool LoadASCII(SALOMEDS::SComponent_ptr, const SALOMEDS::TMPFile &,
                           const char* theURL, bool isMultiFile);
    
    //! inherited methods from SALOMEDS::Driver
    virtual void Close(SALOMEDS::SComponent_ptr IORSComponent);

    //! inherited methods from SALOMEDS::Driver
    virtual char* ComponentDataType();

    //! inherited methods from SALOMEDS::Driver
    virtual char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject, const char* IORString,
                                         CORBA::Boolean isMultiFile, CORBA::Boolean isASCII);

    //! inherited methods from SALOMEDS::Driver
    virtual char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject, const char* aLocalPersistentID,
                                         CORBA::Boolean isMultiFile, CORBA::Boolean isASCII);

    //! inherited methods from SALOMEDS::Driver. Alwais returns false for this module
    virtual bool CanPublishInStudy(CORBA::Object_ptr theIOR);

    //! inherited methods from SALOMEDS::Driver. Does nothing
    virtual SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                                 SALOMEDS::SObject_ptr theSObject, 
                                                 CORBA::Object_ptr theObject,
                                                 const char* theName) throw (SALOME::SALOME_Exception);

    //! inherited methods from SALOMEDS::Driver. Alwais returns false for this module
    virtual CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);

    //! inherited methods from SALOMEDS::Driver. Does nothing
    virtual SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);

    //! inherited methods from SALOMEDS::Driver. Does nothing
    virtual CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);

    //! inherited methods from SALOMEDS::Driver. Does nothing
    virtual SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                            CORBA::Long theObjectID, SALOMEDS::SObject_ptr theObject);

    //! inherited methods from Engines::EngineComponent. Returns module version.
    virtual char* getVersion();

    //! Implementation of PARAVIS_Gen interface
    virtual PARAVIS::string_array* GetClassesList();

    //! Implementation of PARAVIS_Gen interface
    virtual PARAVIS_Base_ptr CreateClass(const char* theClassName);

    //! Implementation of PARAVIS_Gen interface
    virtual void GetConnectionParameters(CORBA::Long& theId, 
                                         CORBA::String_out theDHost, CORBA::Long& theDPort,
                                         CORBA::String_out theRHost, CORBA::Long& theRPort,
                                         CORBA::Long& theReversed);


    //! Implementation of PARAVIS_Gen interface
    virtual void ActivateModule();

    virtual void SetCurrentStudy(SALOMEDS::Study_ptr theStudy);

    virtual SALOMEDS::Study_ptr GetCurrentStudy();

    //! Returns current ORB
    static CORBA::ORB_var GetORB() { return myOrb;}

    //! Returns current POA
    static PortableServer::POA_var GetPOA() { return myPoa;}

    //! Returns current implementation instance
    static PARAVIS_Gen_i* GetParavisGenImpl() { return myParavisGenImpl;}

  private:
    static CORBA::ORB_var          myOrb;
    static PortableServer::POA_var myPoa;
    static PARAVIS_Gen_i*          myParavisGenImpl;
    static SALOME_LifeCycleCORBA*  myEnginesLifeCycle;
    static SALOME_NamingService*   myNamingService;
    static QMutex*                 myMutex;

    std::string         myIOR;
    SALOMEDS::Study_var myStudyDocument;
    static SalomeApp_Application* mySalomeApp;

  };

};
#endif
