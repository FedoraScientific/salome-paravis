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
#ifndef __PARAVIS_ENGINE_I_H__
#define __PARAVIS_ENGINE_I_H__

// IDL headers
#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(PARAVIS_Gen)
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "SALOME_Component_i.hxx"

namespace PARAVIS
{
  /*!
   * Implements PARAVIS_Gen interface. 
   * Ativates corresponded interface in GUI main thread and redirects all calls to that interface.
   */
  class PARAVIS_Gen_i : public virtual POA_PARAVIS::PARAVIS_Gen,
		     public virtual ::Engines_Component_i
  {
    //! Reference on corresponded interface in GUI thread
    PARAVIS_Gen_var myParaVisGen;

    //! Empty Constructor
    PARAVIS_Gen_i();
    //! Copy Constructor
    PARAVIS_Gen_i(const PARAVIS::PARAVIS_Gen_i &);
  public:
    //! Constructor
    PARAVIS_Gen_i(SALOME::Session_ptr session,
                  CORBA::ORB_ptr orb,
                  PortableServer::POA_ptr poa,
                  PortableServer::ObjectId * contId, 
                  const char *instanceName, 
                  const char *interfaceName);
    virtual ~PARAVIS_Gen_i();
    
    virtual char* GetIOR();

    virtual void ImportFile(const char* theFileName);

    virtual void ExecuteScript(const char* script);
    
    virtual char* GetTrace();
    
    virtual void SaveTrace(const char* theFileName);
    
    // inherited methods from SALOMEDS::Driver
    virtual SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent, 
                                    const char* theURL, bool isMultiFile);
    
    virtual SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                                         const char* theURL, bool isMultiFile);
    
    virtual bool Load(SALOMEDS::SComponent_ptr,	 const SALOMEDS::TMPFile &,
                      const char* theURL, bool isMultiFile);

    virtual bool LoadASCII(SALOMEDS::SComponent_ptr, const SALOMEDS::TMPFile &,
                           const char* theURL, bool isMultiFile);
    
    virtual void Close(SALOMEDS::SComponent_ptr IORSComponent);

    virtual char* ComponentDataType();

    virtual char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject, const char* IORString,
                                         CORBA::Boolean isMultiFile, CORBA::Boolean isASCII);

    virtual char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject, const char* aLocalPersistentID,
                                         CORBA::Boolean isMultiFile, CORBA::Boolean isASCII);

    virtual bool CanPublishInStudy(CORBA::Object_ptr theIOR);

    virtual SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                                 SALOMEDS::SObject_ptr theSObject, 
                                                 CORBA::Object_ptr theObject,
                                                 const char* theName) throw (SALOME::SALOME_Exception);

    virtual CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);

    virtual SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);

    virtual CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);

    virtual SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                            CORBA::Long theObjectID, SALOMEDS::SObject_ptr theObject);


    virtual PARAVIS::string_array* GetClassesList();
    virtual PARAVIS_Base_ptr CreateClass(const char* theClassName);
    virtual void GetConnectionParameters(CORBA::Long& theId, 
                                         CORBA::String_out theDHost, CORBA::Long& theDPort,
                                         CORBA::String_out theRHost, CORBA::Long& theRPort,
                                         CORBA::Long& theReversed);
                                         
    virtual void ActivateModule();


    virtual void SetCurrentStudy(SALOMEDS::Study_ptr theStudy);
    virtual SALOMEDS::Study_ptr GetCurrentStudy();

    virtual char* getVersion();
  };
};
#endif
