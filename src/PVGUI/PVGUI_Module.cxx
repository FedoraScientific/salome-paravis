// LIGHT : sample (no-corba-engine) SALOME module
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File   : PVGUI_Module.cxx
// Author : Julia DOROVSKIKH
//

#include "PVGUI_Module.h"
#include "PVGUI_ProcessModuleHelper.h"
#include "PVGUI_ViewModel.h"
#include "PVGUI_ViewManager.h"
#include "PVGUI_ViewWindow.h"

#include <SUIT_MessageBox.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <QApplication>
#include <QInputDialog>
#include <QStringList>
#include <QMenu>
#include <QIcon>
#include <QString>

#include <pqOptions.h>
#include <pqApplicationCore.h>
#include <pqActiveServer.h>
#include <pqMainWindowCore.h>
#include <pqObjectBuilder.h>
#include <pqServer.h>
#include <pqServerManagerModel.h>
#include <pqServerResource.h>
#include <pqViewManager.h>
#include <vtkPVMain.h>
#include <vtkProcessModule.h>

/*
 * Make sure all the kits register their classes with vtkInstantiator.
 * Since ParaView uses Tcl wrapping, all of VTK is already compiled in
 * anyway.  The instantiators will add no more code for the linker to
 * collect.
 */

#include <vtkCommonInstantiator.h>
#include <vtkFilteringInstantiator.h>
#include <vtkGenericFilteringInstantiator.h>
#include <vtkIOInstantiator.h>
#include <vtkImagingInstantiator.h>
#include <vtkInfovisInstantiator.h>
#include <vtkGraphicsInstantiator.h>

#include <vtkRenderingInstantiator.h>
#include <vtkVolumeRenderingInstantiator.h>
#include <vtkHybridInstantiator.h>
#include <vtkParallelInstantiator.h>

#include <vtkPVServerCommonInstantiator.h>
#include <vtkPVFiltersInstantiator.h>
#include <vtkPVServerManagerInstantiator.h>
#include <vtkClientServerInterpreter.h>


//----------------------------------------------------------------------------
// ClientServer wrapper initialization functions.
// Taken from ParaView sources (file pqMain.cxx)
extern "C" void vtkCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGenericFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkImagingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkInfovisCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGraphicsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkIOCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkVolumeRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkHybridCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkWidgetsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkParallelCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVServerCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVFiltersCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkXdmfCS_Initialize(vtkClientServerInterpreter *);

//----------------------------------------------------------------------------
void ParaViewInitializeInterpreter(vtkProcessModule* pm)
{
  // Initialize built-in wrapper modules.
  vtkCommonCS_Initialize(pm->GetInterpreter());
  vtkFilteringCS_Initialize(pm->GetInterpreter());
  vtkGenericFilteringCS_Initialize(pm->GetInterpreter());
  vtkImagingCS_Initialize(pm->GetInterpreter());
  vtkInfovisCS_Initialize(pm->GetInterpreter());
  vtkGraphicsCS_Initialize(pm->GetInterpreter());
  vtkIOCS_Initialize(pm->GetInterpreter());
  vtkRenderingCS_Initialize(pm->GetInterpreter());
  vtkVolumeRenderingCS_Initialize(pm->GetInterpreter());
  vtkHybridCS_Initialize(pm->GetInterpreter());
  vtkWidgetsCS_Initialize(pm->GetInterpreter());
  vtkParallelCS_Initialize(pm->GetInterpreter());
  vtkPVServerCommonCS_Initialize(pm->GetInterpreter());
  vtkPVFiltersCS_Initialize(pm->GetInterpreter());
  vtkXdmfCS_Initialize(pm->GetInterpreter());
}

//////////////////////////////////////////////////////////////////////////////
// PVGUI_Module::pqImplementation

class PVGUI_Module::pqImplementation
{
public:
  pqImplementation(QWidget* parent) :
    //AssistantClient(0),
    Core(parent)//,
    //RecentFilesMenu(0),
    //ViewMenu(0),
    //ToolbarsMenu(0)
  {
  }

  ~pqImplementation()
  {
    //delete this->ViewMenu;
    //delete this->ToolbarsMenu;
    //if(this->AssistantClient)
    //  {
    //  this->AssistantClient->closeAssistant();
    //  delete this->AssistantClient;
    //  }
  }

  //QPointer<QAssistantClient> AssistantClient;
  //Ui::MainWindow UI;
  pqMainWindowCore Core;
  //pqRecentFilesMenu* RecentFilesMenu;
  //pqViewMenu* ViewMenu;
  //pqViewMenu* ToolbarsMenu;
  //QLineEdit* CurrentTimeWidget;
  //QSpinBox* CurrentTimeIndexWidget;
  QPointer<pqServer> ActiveServer;
  QString DocumentationDir;

  static vtkPVMain* myPVMain;
  static pqOptions* myPVOptions;
  static PVGUI_ProcessModuleHelper* myPVHelper;
};


vtkPVMain*                 PVGUI_Module::pqImplementation::myPVMain = 0;
pqOptions*                 PVGUI_Module::pqImplementation::myPVOptions = 0;
PVGUI_ProcessModuleHelper* PVGUI_Module::pqImplementation::myPVHelper = 0;

/*!
  \class PVGUI_Module
  \brief Implementation of light (no-CORBA-engine) 
         SALOME module wrapping ParaView GUI.
*/

/*!
  \brief Constructor. Sets the default name for the module.
*/
PVGUI_Module::PVGUI_Module()
  : LightApp_Module( "PARAVIS" ),
    Implementation( 0 )
{
}

/*!
  \brief Destructor.
*/
PVGUI_Module::~PVGUI_Module()
{
}

/*!
  \brief Initialize module. Creates menus, prepares context menu, etc.
  \param app application instance
*/
void PVGUI_Module::initialize( CAM_Application* app )
{
  LightApp_Module::initialize( app );

  /*
  int i = 1;
  while( i ){
    i = i;
  }
  */

  pvInit();

  /*
  createAction( lgLoadFile, tr( "TOP_LOAD_FILE" ), QIcon(), tr( "MEN_LOAD_FILE" ),
                tr( "STB_LOAD_FILE" ), 0, desk, false, this, SLOT( onLoadFile() ) );
  createAction( lgDisplayLine, tr( "TOP_DISPLAY_LINE" ), QIcon(), tr( "MEN_DISPLAY_LINE" ),
                tr( "STB_DISPLAY_LINE" ), 0, desk, false, this, SLOT( onDisplayLine() ) );
  createAction( lgEraseLine, tr( "TOP_ERASE_LINE" ), QIcon(), tr( "MEN_ERASE_LINE" ),
                tr( "STB_ERASE_LINE" ), 0, desk, false, this, SLOT( onEraseLine() ) );
  createAction( lgSaveFile, tr( "TOP_SAVE_FILE" ), QIcon(), tr( "MEN_SAVE_FILE" ),
                tr( "STB_SAVE_FILE" ), 0, desk, false, this, SLOT( onSaveFile() ) );
  createAction( lgEditLine, tr( "TOP_EDIT_LINE" ), QIcon(), tr( "MEN_EDIT_LINE" ),
                tr( "STB_EDIT_LINE" ), 0, desk, false, this, SLOT( onEditLine() ) );
  createAction( lgAddLine,  tr( "TOP_ADD_LINE" ),  QIcon(), tr( "MEN_ADD_LINE" ),
                tr( "STB_ADD_LINE" ),  0, desk, false, this, SLOT( onAddLine() ) );
  createAction( lgDelLine,  tr( "TOP_DEL_LINE" ),  QIcon(), tr( "MEN_DEL_LINE" ),
                tr( "STB_DEL_LINE" ),  0, desk, false, this, SLOT( onDelLine() ) );
  createAction( lgClear,    tr( "TOP_CLEAR_ALL" ), QIcon(), tr( "MEN_CLEAR_ALL" ),
                tr( "STB_CLEAR_ALL" ), 0, desk, false, this, SLOT( onClear() ) );

  int aFileMnu = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( separator(), aFileMnu, -1, 10 );
  createMenu( lgLoadFile,  aFileMnu, 10 );
  createMenu( lgSaveFile,  aFileMnu, 10 );

  int aLightMnu = createMenu( tr( "MEN_LIGHT" ), -1, -1, 50 );
  createMenu( lgAddLine,      aLightMnu, 10 );
  createMenu( lgEditLine,     aLightMnu, 10 );
  createMenu( lgDelLine,      aLightMnu, 10 );
  createMenu( separator(),    aLightMnu, -1, 10 );
  createMenu( lgClear,        aLightMnu, 10 );

  QString rule = "(client='ObjectBrowser' or client='OCCViewer') and selcount=1 and type='TextLine' and !empty";

  popupMgr()->insert ( action( lgDisplayLine ), -1, 0 );
  popupMgr()->setRule( action( lgDisplayLine ), rule + " and !visible"  );

  popupMgr()->insert ( action( lgEraseLine ), -1, 0 );
  popupMgr()->setRule( action( lgEraseLine ), rule + " and activeView='OCCViewer' and visible"  );

  rule = "client='ObjectBrowser' and selcount=1 and type='TextLine'";

  popupMgr()->insert ( action( lgEditLine ), -1, 0 );
  popupMgr()->setRule( action( lgEditLine ), rule  );

  popupMgr()->insert ( action( lgAddLine ),  -1, 0 );
  popupMgr()->setRule( action( lgAddLine ),  rule );

  popupMgr()->insert ( separator(),          -1, 0 );

  popupMgr()->insert ( action( lgDelLine ),  -1, 0 );
  popupMgr()->setRule( action( lgDelLine ),  rule );

  rule = "client='ObjectBrowser'";

  popupMgr()->insert ( action( lgClear ),    -1, 0 );
  popupMgr()->setRule( action( lgClear ),    rule );*/
}

/*!
  \brief Get list of compliant dockable GUI elements
  \param m map to be filled in ("type":"default_position")
*/
void PVGUI_Module::windows( QMap<int, int>& m ) const
{
  m.insert( LightApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  // TODO: creation of Python console leads to SIGSEGV on Python initialization...
  //m.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
  // ParaView diagnostic output redirected here
  m.insert( LightApp_Application::WT_LogWindow, Qt::BottomDockWidgetArea );
}

/*!
  \brief Create custom popup menu selection object.
  \return new selected object
*/
/*LightApp_Selection* PVGUI_Module::createSelection() const
{
  return new PVGUI_Selection();
}*/

/*!
  \brief Create data model.
  \return module specific data model
*/
/*CAM_DataModel* PVGUI_Module::createDataModel()
{
  return new PVGUI_DataModel( this );
}*/

/*!
  \brief Static method, performs initialization of ParaView session.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVGUI_Module::pvInit()
{
  if ( !pqImplementation::myPVMain ){
    // Obtain command-line arguments
    int argc = 0;
    QStringList args = QApplication::arguments();
    char** argv = new char*[args.size()];
    for ( QStringList::const_iterator it = args.begin(); argc < 1 && it != args.end(); it++, argc++ )
      argv[argc] = strdup( (*it).toLatin1().constData() );

    vtkPVMain::SetInitializeMPI(0);  // pvClient never runs with MPI.
    vtkPVMain::Initialize(&argc, &argv); // Perform any initializations.

    // TODO: Set plugin dir from preferences
    //QApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    pqImplementation::myPVMain = vtkPVMain::New();
    if ( !pqImplementation::myPVOptions )
      pqImplementation::myPVOptions = pqOptions::New();
    if ( !pqImplementation::myPVHelper )
      pqImplementation::myPVHelper = PVGUI_ProcessModuleHelper::New();

    pqImplementation::myPVOptions->SetProcessType(vtkPVOptions::PVCLIENT);

    // This creates the Process Module and initializes it.
    int ret = pqImplementation::myPVMain->Initialize(pqImplementation::myPVOptions, 
                                                     pqImplementation::myPVHelper, 
                                                     ParaViewInitializeInterpreter,
                                                     argc, argv);
    if (!ret){
      // Tell process module that we support Multiple connections.
      // This must be set before starting the event loop.
      vtkProcessModule::GetProcessModule()->SupportMultipleConnectionsOn();
      ret = pqImplementation::myPVHelper->Run(pqImplementation::myPVOptions);
    }

    delete[] argv;
    return !ret;
  }
  
  return true;
}
 
/*!
  \brief Static method, cleans up ParaView session at application exit.
*/
void PVGUI_Module::pvShutdown()
{
  // TODO...
}  

/*!
  \brief Shows (toShow = true) or hides ParaView view window
*/
void PVGUI_Module::showView( bool toShow )
{
  // TODO: check if ParaView view already exists
  if ( !Implementation ){
    LightApp_Application* anApp = getApp();
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    PVGUI_ViewManager* viewMgr = new PVGUI_ViewManager( anApp->activeStudy(), anApp->desktop() );
    anApp->addViewManager( viewMgr );
    connect( viewMgr, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
             anApp, SLOT( onCloseView( SUIT_ViewManager* ) ) );
    //connect( viewMgr, SIGNAL( viewCreated( SUIT_ViewWindow* ) ), vm, SLOT( onViewCreated( SUIT_ViewWindow* ) ) );
    //connect( viewMgr, SIGNAL( deleteView( SUIT_ViewWindow* ) ), this, SLOT( onViewDeleted( SUIT_ViewWindow* ) ) );
    SUIT_ViewWindow* wnd = viewMgr->createViewWindow();  

    // Simulate ParaView client main window
    Implementation = new pqImplementation( wnd );
    PVGUI_ViewWindow* pvWnd = dynamic_cast<PVGUI_ViewWindow*>( wnd );
    pvWnd->setMultiViewManager( &Implementation->Core.multiViewManager() );

    pvCreateActions();
    setupDockWidgets();
  }
}
 
/*!
  \brief Create actions for ParaView GUI operations
  duplicating menus and toolbars in pqMainWindow ParaView class
*/
void PVGUI_Module::pvCreateActions()
{
  // TODO...
  SUIT_Desktop* desk = application()->desktop();

  // TEST
  int actionManagePlugins = 999;
  createAction( actionManagePlugins, tr( "TOP_MANAGE_PLUGINS" ), QIcon(), tr( "MEN_MANAGE_PLUGINS" ),
                tr( "STB_MANAGE_PLUGINS" ), 0, desk, false, &Implementation->Core, SLOT( onManagePlugins() ) );
  int aPVMnu = createMenu( tr( "MEN_TEST_PARAVIEW" ), -1, -1, 50 );
  createMenu( actionManagePlugins, aPVMnu, 10 );
}


/*!
  \brief Create dock widgets for ParaView widgets such as object inspector, pipeline browser, etc.
*/
void PVGUI_Module::setupDockWidgets()
{
}

/*!
  \brief Returns the ParaView multi-view manager.
*/
pqViewManager* PVGUI_Module::getMultiViewManager() const
{
  pqViewManager* aMVM = 0; 
  if ( Implementation )
    aMVM = &Implementation->Core.multiViewManager();
  return aMVM;
}


/*!
  \brief Activate module.
  \param study current study
  \return \c true if activaion is done successfully or 0 to prevent
  activation on error
*/
bool PVGUI_Module::activateModule( SUIT_Study* study )
{
  bool isDone = LightApp_Module::activateModule( study );
  if ( !isDone ) return false;

  setMenuShown( true );

  showView( true );

  // Make default server connection
  if ( Implementation )
    Implementation->Core.makeDefaultConnectionIfNoneExists();

  return isDone;
}


/*!
  \brief Deactivate module.
  \param study current study
  \return \c true if deactivaion is done successfully or 0 to prevent
  deactivation on error
*/
bool PVGUI_Module::deactivateModule( SUIT_Study* study )
{
  // hide menus
  setMenuShown( false );

  return LightApp_Module::deactivateModule( study );
}

/*!
  \fn CAM_Module* createModule();
  \brief Export module instance (factory function).
  \return new created instance of the module
*/

#ifdef WNT
#define PVGUI_EXPORT __declspec(dllexport)
#else   // WNT
#define PVGUI_EXPORT
#endif  // WNT

extern "C" {
  PVGUI_EXPORT CAM_Module* createModule() {
    return new PVGUI_Module();
  }
}
