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
//  File   : PV_Events.h
//  Author : Vitaly Smetannikov 
//  Module : PARAVIS
//

#ifndef PV_EVENTS_H
#define PV_EVENTS_H

#include "SALOME_Event.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include "SUIT_Session.h"


/*!
 * Definition of events
 */
namespace PARAVIS {
  
  /*!
   * Event which returns SalomeApp_Application instance which corresponds to th given study ID
   */
  struct TGetGUIApplication: public SALOME_Event
  {
    int myStudyId;
    typedef SalomeApp_Application* TResult;
    TResult myResult;
    
    TGetGUIApplication(const int theStudyId):myStudyId(theStudyId), myResult(0) {}
    
    virtual void Execute()
    {
      MESSAGE("Find application for study with id = : " << myStudyId);
      SUIT_Session* aSession = SUIT_Session::session();
      QList<SUIT_Application*> anApplications = aSession->applications();
      for (int i = 0; i < anApplications.count() && !myResult; i++ ){
        if ( anApplications[i]->activeStudy() && anApplications[i]->activeStudy()->id() == myStudyId )
          myResult = dynamic_cast<SalomeApp_Application*>( anApplications[i] );
      }
      if ( !myResult ) {
        MESSAGE("Error: application is not found for study with id = : " << myStudyId);
      }
    }
  };
  
  /*!
   * Parent event for all events what is going to work with PARAVIS module instance
   */
  struct TModuleEvent: public SALOME_Event
  {
    SalomeApp_Application* myApp;

    TModuleEvent(SalomeApp_Application* theApp ):myApp(theApp) {}

      //! Returns pointer on PARAVIS module instance
    PVGUI_Module* getModule() 
    {
      PVGUI_Module* aPVModule = 0;
      CAM_Application::ModuleList aList = myApp->modules();
      foreach(CAM_Module* aModule, aList) {
        if (aModule->moduleName() == "ParaViS") {
          aPVModule = dynamic_cast<PVGUI_Module*>(aModule);
          break;
        }
      }
      return aPVModule;
    }
  };

  /*!
   * Event which activates PARAVIS module if it is not active
   */
  struct TActivateModule: public TModuleEvent
  {
    TActivateModule(SalomeApp_Application* theApp ):TModuleEvent(theApp) {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if ((aPVModule == 0) || (myApp->activeModule() != aPVModule)) {
        myApp->activateModule("ParaViS");
      }
    }
  };

  /*!
   * Returns trace string collected in PARAVIS module in current session
   */
  struct TGetTrace: public TModuleEvent
  {
    typedef std::string TResult;
    TResult myResult;
    TGetTrace(SalomeApp_Application* theApp) :
      TModuleEvent(theApp)
    {
      myResult = "";
    }

    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        myResult = qPrintable(aPVModule->getTraceString());
    }
  };

  /*!
   * Parent event for all operations with files across PARAVIS module
   */
  struct TParavisFileEvent: public TModuleEvent
  {
    const char* myName;

    TParavisFileEvent(SalomeApp_Application* theApp, const char* theFileName ) :
      TModuleEvent(theApp), myName(theFileName)
    {}
  };

  /*!
   * Event to save trace string inte disk file
   */
  struct TSaveTrace: public TParavisFileEvent
  {
    TSaveTrace(SalomeApp_Application* theApp, const char* theFileName ) :
      TParavisFileEvent(theApp, theFileName)
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        aPVModule->saveTrace(myName);
    }
  };


  /*!
   * Event to import file to PARAVIS
   */
  struct TImportFile: public TParavisFileEvent
  {
    TImportFile(SalomeApp_Application* theApp, const char* theFileName ) :
      TParavisFileEvent(theApp, theFileName)
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        aPVModule->openFile(myName);
    }
  };

  /*!
   * Event to execute a script to PARAVIS
   */
  struct TExecuteScript: public TParavisFileEvent
  {
    TExecuteScript(SalomeApp_Application* theApp, const char* theFileName ) :
      TParavisFileEvent(theApp, theFileName)
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        aPVModule->executeScript(myName);
    }
  };

  /*!
   * Event to save current Paraview state.
   * This event is used for Study saving
   */
  struct TSaveStateFile: public TParavisFileEvent
  {
    TSaveStateFile(SalomeApp_Application* theApp, const char* theFileName ) :
      TParavisFileEvent(theApp, theFileName)      
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        aPVModule->saveParaviewState(myName);
    }
  };


  /*!
   * Event to restore Paraview state from disk file.
   * This event is used for Study restoring
   */
  struct TLoadStateFile: public TParavisFileEvent
  {
    TLoadStateFile(SalomeApp_Application* theApp, const char* theFileName ) :
      TParavisFileEvent(theApp, theFileName)      
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        aPVModule->loadParaviewState(myName);
    }
  };

  /*!
   * Returns currently active Paraview server
   */
  struct TGetActiveServer: public TModuleEvent
  {
    typedef pqServer* TResult;
    TResult myResult;

    TGetActiveServer(SalomeApp_Application* theApp ) :
      TModuleEvent(theApp), myResult(0)   
    {}
    
    virtual void Execute()
    {
      PVGUI_Module* aPVModule = getModule();
      if (aPVModule)
        myResult = aPVModule->getActiveServer();
    }
  };

};


#endif  //PV_EVENTS_H
