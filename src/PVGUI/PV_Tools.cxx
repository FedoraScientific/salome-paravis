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
//  VISU OBJECT : interactive object for VISU entities implementation
//  File   : PV_Tools.cxx
//  Author : Alexey PETROV
//  Module : VISU
//
#include "PV_Tools.h"  
#include "PARAVIS_Gen_i.hh"

#include <omnithread.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include <SALOME_NamingService.hxx>

#include <QString>

namespace PARAVIS
{
//----------------------------------------------------------------------------
  SALOMEDS::ListOfFileNames* GetListOfFileNames(const QStringList& theFileNames)
  {
    SALOMEDS::ListOfFileNames_var aListOfFileNames = new SALOMEDS::ListOfFileNames;
    if(!theFileNames.isEmpty()){
      aListOfFileNames->length(theFileNames.size());
      int i = 0;
      foreach (QString aName, theFileNames) {
        aListOfFileNames[i] = qPrintable(aName);
        i++;
      }
    }
    return aListOfFileNames._retn();
  }

  //----------------------------------------------------------------------------
  SALOMEDS::SComponent_var FindOrCreateParaVisComponent(SALOMEDS::Study_ptr theStudyDocument)
  {
    SALOMEDS::SComponent_var aSComponent = theStudyDocument->FindComponent("PARAVIS");
    if (aSComponent->_is_nil()) {
      SALOMEDS::StudyBuilder_var aStudyBuilder = theStudyDocument->NewBuilder();
      aStudyBuilder->NewCommand();
      int aLocked = theStudyDocument->GetProperties()->IsLocked();
      if (aLocked) theStudyDocument->GetProperties()->SetLocked(false);
      aSComponent = aStudyBuilder->NewComponent("PARAVIS");
      SALOMEDS::GenericAttribute_var anAttr = 
        aStudyBuilder->FindOrCreateAttribute(aSComponent, "AttributeName");
      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
      
      CORBA::ORB_var anORB = PARAVIS_Gen_i::GetORB();
      SALOME_NamingService *NamingService = new SALOME_NamingService( anORB );
      CORBA::Object_var objVarN = NamingService->Resolve("/Kernel/ModulCatalog");
      SALOME_ModuleCatalog::ModuleCatalog_var Catalogue  = 
        SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
      SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent( "PARAVIS" );
      if ( !CORBA::is_nil(Comp) ) {
        aName->SetValue( Comp->componentusername() );
      }
      
      anAttr = aStudyBuilder->FindOrCreateAttribute(aSComponent, "AttributePixMap");
      SALOMEDS::AttributePixMap_var aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
      aPixmap->SetPixMap( "pqAppIcon16.png" );
      
      PARAVIS_Gen_var aParaVisGen = PARAVIS_Gen_i::GetParavisGenImpl()->_this();
      aStudyBuilder->DefineComponentInstance(aSComponent, aParaVisGen);
      if (aLocked) 
        theStudyDocument->GetProperties()->SetLocked(true);
      aStudyBuilder->CommitCommand();
    }
    return aSComponent;
  }

  PARAVIS_Base_i* GET_SERVANT(CORBA::Object_ptr aObj) {
    //dynamic_cast<PARAVIS_Base_i*>(PARAVIS_Gen_i::GetPOA()->reference_to_servant(OBJ).in())
    if (CORBA::is_nil(aObj))
      return NULL;
    try {
      PortableServer::POA_ptr aPOA = PARAVIS_Gen_i::GetPOA();
      PortableServer::ServantBase_var aServant = aPOA->reference_to_servant(aObj);
      return dynamic_cast<PARAVIS_Base_i*>(aServant.in());
    } catch (...) {
      MESSAGE("GET_SERVANT: Unknown exception!!!");
    }
    return NULL;
  }
};
