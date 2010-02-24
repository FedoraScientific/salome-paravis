// PARAVIS : ParaView wrapper SALOME module
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

#include <vtkPython.h> // Python first
#include "PVGUI_Module.h"

#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(VISU_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS)


#include "PARAVIS_Gen_i.hh"

#include "PVGUI_Module_impl.h"
#include "PVGUI_ViewModel.h"
#include "PVGUI_ViewManager.h"
#include "PVGUI_ViewWindow.h"
#include "PVGUI_Tools.h"
//#include "PVGUI_Trace.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>

// SALOME Includes
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOMEDS_SObject.hxx"

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>
#include <SALOMEDS_Tool.hxx>
#include <PyInterp_Dispatcher.h>

#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QInputDialog>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QToolBar>
#include <QTextStream>
#include <QShortcut>

#include <pqApplicationCore.h>
#include <pqActiveView.h>
#include <pqObjectBuilder.h>
#include <pqOptions.h>
#include <pqRenderView.h>
#include <pqServer.h>
#include <pqUndoStack.h>
#include <pqVCRController.h>
#include <pqViewManager.h>
#include <pqPipelineSource.h>
#include <vtkPVMain.h>
#include <vtkProcessModule.h>
#include <pqParaViewBehaviors.h>
#include <pqHelpReaction.h>
#include <vtkOutputWindow.h>
#include <pqPluginManager.h>
#include <vtkPVPluginInformation.h>
#include <pqSettings.h>
#include <pqPythonDialog.h>
#include <pqPythonManager.h>
#include <pqPythonShell.h>
#include "pqBrandPluginsLoader.h"
#include <pqLoadDataReaction.h>

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

#include <pqAlwaysConnectedBehavior.h>
#include <pqApplicationCore.h>
#include <pqAutoLoadPluginXMLBehavior.h>
#include <pqCommandLineOptionsBehavior.h>
#include <pqCrashRecoveryBehavior.h>
#include <pqDataTimeStepBehavior.h>
#include <pqDefaultViewBehavior.h>
#include <pqDeleteBehavior.h>
#include <pqPersistentMainWindowStateBehavior.h>
#include <pqPluginActionGroupBehavior.h>
#include <pqPluginDockWidgetsBehavior.h>
#include <pqPluginManager.h>
#include <pqPVNewSourceBehavior.h>
//#include <pqQtMessageHandlerBehavior.h>
#include <pqSpreadSheetVisibilityBehavior.h>
#include <pqStandardViewModules.h>
#include <pqUndoRedoBehavior.h>
#include <pqViewFrameActionsBehavior.h>


//----------------------------------------------------------------------------
pqApplicationCore* PVGUI_Module::pqImplementation::Core = 0;
PVGUI_OutputWindowAdapter* PVGUI_Module::pqImplementation::OutputWindowAdapter = 0;
QPointer<pqHelpWindow> PVGUI_Module::pqImplementation::helpWindow = 0;

PVGUI_Module* ParavisModule = 0;

/*!
  \mainpage

  <h2>Building and installing PARAVIS</h2>
  As any other SALOME module, PARAVIS requires PARAVIS_ROOT_DIR environment variable to be set to PARAVIS
  installation directory.
  Other variables needed for correct detection of ParaView location:
  \li PVHOME - points at the ParaView installation directory tree
  \li PVVERSION - number of ParaView version

  It also requires common SALOME environment including GUI_ROOT_DIR and other prerequsites.


  PARAVIS module can be launched using the following commands:
  \li Full SALOME configuration
  \code
  runSalome --modules="PARAVIS"
  \endcode

  <h2>ParaView GUI integration</h2>
  <h3>ParaView GUI integration overview</h3>

  The main idea is to reuse ParaView GUI internal logic as much as possible, providing a layer 
  between it and SALOME GUI that hides the following SALOME GUI implementation details from ParaView:

  \li SALOME GUI executable and Qt event loop
  \li SALOME GUI desktop
  \li Dock windows areas
  \li SALOME menu and toolbar managers

  Major part of the integration is implemented in PVGUI_Module class.

  <h3>ParaView client initalization</h3>

  ParaView client initalization is performed when an instance of PVGUI_Module class has been created 
  and \link PVGUI_Module::initialize() PVGUI_Module::initialize()\endlink method is called by SALOME GUI.
  The actual client start-up is done in \link PVGUI_Module::pvInit() PVGUI_Module::pvInit()\endlink method. 
  

  <h3>Multi-view manager</h3>

  SALOME GUI requires that each kind of view be implemnted with help of (at least) three classes. For ParaView multi-view manager 
  these are:

  \li PVGUI_ViewManager - view manager class
  \li PVGUI_Viewer      - view model class
  \li PVGUI_ViewWindow  - view window class that acts as a parent for %pqViewManager

  Single instances of PVGUI_ViewManager and PVGUI_ViewWindow classes are created by \link PVGUI_Module::showView() 
  PVGUI_Module::showView()\endlink method upon the first PARAVIS module activation. The same method hides the multi-view manager 
  when the module is deactivated (the user switches to another module or a study is closed). 
  A special trick is used to make PVGUI_ViewWindow the parent of %pqViewManager widget. It is created initally by %pqMainWindowCore
  with the desktop as a parent, so when it is shown PVGUI_ViewWindow instance is passed to its setParent() method. In  
  \link PVGUI_ViewWindow::~PVGUI_ViewWindow() PVGUI_ViewWindow::~PVGUI_ViewWindow()\endlink the parent is nullified to avoid deletion
  of %pqViewManager widget that would break %pqMainWindowCore class.

  <h3>ParaView plugins</h3>
  ParaView server and client plugins are managed by %pqMainWindowCore slots that has full access to PARAVIS menus and toolbars. 
  As a result they appears automatically in PARAVIS menus and toolbars if loaded successfully.
*/

/*!
  \class PVGUI_Module
  \brief Implementation 
         SALOME module wrapping ParaView GUI.
*/

/*!
  \brief Constructor. Sets the default name for the module.
*/
PVGUI_Module::PVGUI_Module()
  : SalomeApp_Module( "PARAVIS" ),
    LightApp_Module( "PARAVIS" ),
    Implementation( 0 ),
    mySelectionControlsTb( -1 ),
    mySourcesMenuId( -1 ),
    myFiltersMenuId( -1 ),
    myMacrosMenuId(-1),
    myToolbarsMenuId(-1),
    myOldMsgHandler(0)
{
  ParavisModule = this;
}

/*!
  \brief Destructor.
*/
PVGUI_Module::~PVGUI_Module()
{
}



/*!
  \brief Initialize module. Creates menus, prepares context menu, etc.
  \param app SALOME GUI application instance
*/
void PVGUI_Module::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // Uncomment to debug ParaView initialization
  // "aa" used instead of "i" as GDB doesn't like "i" variables :)
  /*
  int aa = 1;
  while( aa ){
    aa = aa;
  }
  */
  
  // Initialize ParaView client
  pvInit();

  // Create GUI elements (menus, toolbars, dock widgets)
  if ( !Implementation ){
    SalomeApp_Application* anApp = getApp();

    // Simulate ParaView client main window
    Implementation = new pqImplementation( anApp->desktop() );

    setupDockWidgets();
    
    pvCreateActions();
    pvCreateToolBars();
    pvCreateMenus();

    // new pqParaViewBehaviors(anApp->desktop(), this);
    // Has to be replaced in order to exclude using of pqQtMessageHandlerBehaviour
    //  Start pqParaViewBehaviors
    // Register ParaView interfaces.
    pqPluginManager* pgm = pqApplicationCore::instance()->getPluginManager();

    // * adds support for standard paraview views.
    pgm->addInterface(new pqStandardViewModules(pgm));

    // Load plugins distributed with application.
    pqApplicationCore::instance()->loadDistributedPlugins();

    // Define application behaviors.
    //new pqQtMessageHandlerBehavior(this);
    new pqDataTimeStepBehavior(this);
    new pqViewFrameActionsBehavior(this);
    new pqSpreadSheetVisibilityBehavior(this);
    new pqDefaultViewBehavior(this);
    new pqAlwaysConnectedBehavior(this);
    new pqPVNewSourceBehavior(this);
    new pqDeleteBehavior(this);
    new pqUndoRedoBehavior(this);
    new pqCrashRecoveryBehavior(this);
    new pqAutoLoadPluginXMLBehavior(this);
    new pqPluginDockWidgetsBehavior(getApp()->desktop());
    new pqPluginActionGroupBehavior(getApp()->desktop());
    new pqCommandLineOptionsBehavior(this);
    new pqPersistentMainWindowStateBehavior(getApp()->desktop());

    // Setup quick-launch shortcuts.
    QShortcut *ctrlSpace = new QShortcut(Qt::CTRL + Qt::Key_Space,
      getApp()->desktop());
    QObject::connect(ctrlSpace, SIGNAL(activated()),
      pqApplicationCore::instance(), SLOT(quickLaunch()));
    QShortcut *altSpace = new QShortcut(Qt::ALT + Qt::Key_Space,
      getApp()->desktop());
    QObject::connect(altSpace, SIGNAL(activated()),
      pqApplicationCore::instance(), SLOT(quickLaunch()));
    //  End pqParaViewBehaviors
    

    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    QString aPath = resMgr->stringValue("resources", "PARAVIS", QString());
    if (!aPath.isNull()) {
      pqImplementation::Core->loadConfiguration(aPath + QDir::separator() + "ParaViewFilters.xml");
      pqImplementation::Core->loadConfiguration(aPath + QDir::separator() + "ParaViewReaders.xml");
      pqImplementation::Core->loadConfiguration(aPath + QDir::separator() + "ParaViewSources.xml");
      pqImplementation::Core->loadConfiguration(aPath + QDir::separator() + "ParaViewWriters.xml");
    }
    // Now that we're ready, initialize everything ...


    // Force creation of engine
    PARAVIS::GetParavisGen(this);
    updateObjBrowser();
  }

  // Initialize list of toolbars
  QCoreApplication::processEvents();
  QList<QToolBar*> aBars = getParaViewToolbars();
  foreach (QToolBar* aBar, aBars) {
    myToolbarState[aBar] = true;
  }

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool isStop = aResourceMgr->booleanValue( "PARAVIS", "stop_trace", false );
  if(!isStop) 
    QTimer::singleShot(50, this, SLOT(activateTrace()));
}


void PVGUI_Module::activateTrace()
{
  PyInterp_Dispatcher* aDispatcher = PyInterp_Dispatcher::Get();
  if (aDispatcher->IsBusy()) {
    QTimer::singleShot(50, this, SLOT(activateTrace()));
    return;
  }

  pqPythonDialog* pyDiag = 0;
  pqPythonManager* manager = qobject_cast<pqPythonManager*>(
                             pqApplicationCore::instance()->manager("PYTHON_MANAGER"));
  if (manager)  {
    pyDiag = manager->pythonShellDialog();
  }
  if (pyDiag) {
    pyDiag->runString("try:\n"
                      "  from paraview import smtrace\n"
                      "  smtrace.start_trace()\n"
                      "  print 'Trace started.'\n"
                      "except: raise RuntimeError('could not import paraview.smtrace')\n");
  }
}



/*!
  \brief Get list of compliant dockable GUI elements
  \param m map to be filled in ("type":"default_position")
*/
void PVGUI_Module::windows( QMap<int, int>& m ) const
{
  m.insert( LightApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  m.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
  // ParaView diagnostic output redirected here
  m.insert( LightApp_Application::WT_LogWindow, Qt::BottomDockWidgetArea );
}

/*!
  \brief Static method, performs initialization of ParaView session.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVGUI_Module::pvInit()
{
  if ( !pqImplementation::Core ){
    // Obtain command-line arguments
    int argc = 0;
    QStringList args = QApplication::arguments();
    char** argv = new char*[args.size()];
    for ( QStringList::const_iterator it = args.begin(); argc < 1 && it != args.end(); it++, argc++ ) {
      argv[argc] = strdup( (*it).toLatin1().constData() );
    }
    pqImplementation::Core = new pqPVApplicationCore (argc, argv);
    if (pqImplementation::Core->getOptions()->GetHelpSelected() ||
        pqImplementation::Core->getOptions()->GetUnknownArgument() ||
        pqImplementation::Core->getOptions()->GetErrorMessage() ||
        pqImplementation::Core->getOptions()->GetTellVersion()) {
      return false;
    }
    // VSV: Code from Initializer - it seems that it does nothing

    // Not sure why this is needed. Andy added this ages ago with comment saying
    // needed for Mac apps. Need to check that it's indeed still required.
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Plugins");
    QApplication::addLibraryPath(dir.absolutePath());
    // Load required application plugins.
    QString plugin_string = "";
    QStringList plugin_list = plugin_string.split(';',QString::SkipEmptyParts);
    pqBrandPluginsLoader loader;
    if (loader.loadPlugins(plugin_list) == false) {
      printf("Failed to load required plugins for this application\n");
      return false;
    }

    // Load optional plugins.
    plugin_string = "";
    plugin_list = plugin_string.split(';',QString::SkipEmptyParts);
    loader.loadPlugins(plugin_list, true); //quietly skip not-found plugins.

    // End of Initializer code

    //qInstallMsgHandler(0); // switch off standard Paraview message handler
    pqImplementation::OutputWindowAdapter = PVGUI_OutputWindowAdapter::New();
    vtkOutputWindow::SetInstance(pqImplementation::OutputWindowAdapter);

    //pqPluginManager* pgm = pqApplicationCore::instance()->getPluginManager();
    //pgm->loadExtensions(NULL);

    new pqViewManager(); // it registers as "MULTIVIEW_MANAGER on creation
    
    if (argc == 1)
      free(argv[0]); // because in creation argc < 1
    delete[] argv;
  }
  
  return true;
}
 
/*!
  \brief Shows (toShow = true) or hides ParaView view window
*/
void PVGUI_Module::showView( bool toShow )
{
  SalomeApp_Application* anApp = getApp();
  PVGUI_ViewManager* viewMgr =
    dynamic_cast<PVGUI_ViewManager*>( anApp->getViewManager( PVGUI_Viewer::Type(), false ) );
  if ( !viewMgr ) {
    viewMgr = new PVGUI_ViewManager( anApp->activeStudy(), anApp->desktop() );
    anApp->addViewManager( viewMgr );
    connect( viewMgr, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
             anApp, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  }

  PVGUI_ViewWindow* pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->getActiveView() );
  if ( !pvWnd ) {
    pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->createViewWindow() );
  }

  pvWnd->setShown( toShow );
}


// void PVGUI_Module::connectToPlay()
// {
//   connect( action(PlayId), SIGNAL( triggered() ), &Implementation->Core.VCRController(), SLOT( onPlay() ) );
// }


/*!
  \brief Slot to show help for proxy.
*/
void PVGUI_Module::showHelpForProxy( const QString& proxy )
{
  //pqHelpReaction::showHelp(QString("qthelp://paraview.org/paraview/%1.html").arg(proxy));
  showHelp(QString("qthelp://paraview.org/paraview/%1.html").arg(proxy));
}

void PVGUI_Module::showParaViewHelp()
{
  showHelp(QString());
}

void PVGUI_Module::showHelp(const QString& url)
{
  if (pqImplementation::helpWindow) {
   // raise assistant window;
    pqImplementation::helpWindow->show();
    pqImplementation::helpWindow->raise();
    if (!url.isEmpty()) {
      pqImplementation::helpWindow->showPage(url);
    }
    return;
  }

 // * Discover help project files from the resources.
  QString aPVHome(getenv("PVHOME"));
  if (aPVHome.isNull()) {
    qWarning("Wariable PVHOME is not defined");
    return;
  }
  QChar aSep = QDir::separator();
  QString aFile =  aPVHome + aSep + "doc" + aSep + "paraview.qch";

  if (!QFile::exists(aFile)) {
    qWarning("Help file do not found");
    return;
  }
  
  pqImplementation::helpWindow = new pqHelpWindow(QString("ParaView Online Help"), getApp()->desktop());
  QString namespace_name = pqImplementation::helpWindow->registerDocumentation(aFile);

  pqImplementation::helpWindow->showHomePage(namespace_name);
  pqImplementation::helpWindow->show();
  pqImplementation::helpWindow->raise();
  if (!url.isEmpty()) {
    pqImplementation::helpWindow->showPage(url);
  }
}



/*!
  \brief Slot to show the waiting state.
*/
void PVGUI_Module::onPreAccept()
{
  getApp()->desktop()->statusBar()->showMessage(tr("STB_PREACCEPT"));
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

/*!
  \brief Slot to show the ready state.
*/
void PVGUI_Module::onPostAccept()
{
  getApp()->desktop()->statusBar()->showMessage(tr("STB_POSTACCEPT"), 2000);
  QTimer::singleShot(0, this, SLOT(endWaitCursor()));
}

/*!
  \brief Slot to switch off wait cursor.
*/
void PVGUI_Module::endWaitCursor()
{
  QApplication::restoreOverrideCursor();
}

/*!
  \brief Returns the ParaView multi-view manager.
*/
pqViewManager* PVGUI_Module::getMultiViewManager() const
{
  return qobject_cast<pqViewManager*>(pqApplicationCore::instance()->manager("MULTIVIEW_MANAGER"));
}


static void ParavisMessageOutput(QtMsgType type, const char *msg)
{
  switch(type)
    {
  case QtDebugMsg:
    vtkOutputWindow::GetInstance()->DisplayText(msg);
    break;
  case QtWarningMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
  case QtCriticalMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
  case QtFatalMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
    }
}



/*!
  \brief Activate module.
  \param study current study
  \return \c true if activaion is done successfully or 0 to prevent
  activation on error
*/
bool PVGUI_Module::activateModule( SUIT_Study* study )
{
  myOldMsgHandler = qInstallMsgHandler(ParavisMessageOutput);

  bool isDone = SalomeApp_Module::activateModule( study );
  if ( !isDone ) return false;

  showView( true );
  if ( mySourcesMenuId != -1 ) menuMgr()->show(mySourcesMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myFiltersMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myMacrosMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myToolbarsMenuId);
  setMenuShown( true );
  setToolShown( true );

  if (myToolbarState.size() > 0) {
    SUIT_Desktop* desk = application()->desktop();
    QList<QToolBar*> aToolbars = myToolbarState.keys();
    foreach(QToolBar* aBar, aToolbars) {
      aBar->setParent(desk);
      aBar->setVisible(myToolbarState[aBar]);
    }
  }

  restoreDockWidgetsState();

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
  if (pqImplementation::helpWindow) {
    pqImplementation::helpWindow->hide();
  }
  showView( false );

  // hide menus
  menuMgr()->hide(mySourcesMenuId);
  menuMgr()->hide(myFiltersMenuId);
  menuMgr()->hide(myMacrosMenuId);
  menuMgr()->hide(myToolbarsMenuId);
  setMenuShown( false );
  setToolShown( false );

  saveDockWidgetsState();

  // hide toolbars
  QList<QToolBar*> aToolbars = myToolbarState.keys();
  foreach(QToolBar* aBar, aToolbars) {
    myToolbarState[aBar] = aBar->isVisible();
    aBar->hide();
    aBar->setParent(0);
  }

  if (myOldMsgHandler)
    qInstallMsgHandler(myOldMsgHandler);
  return SalomeApp_Module::deactivateModule( study );
}


/*!
  \brief Called when application is closed.

  Process finalize application functionality from ParaView in order to save server settings
  and nullify application pointer if the application is being closed.

  \param theApp application
*/
void PVGUI_Module::onApplicationClosed( SUIT_Application* theApp )
{
  pqApplicationCore::instance()->settings()->sync();
  int aAppsNb = SUIT_Session::session()->applications().size();
  if (aAppsNb == 1) {
    deleteTemporaryFiles();
    //pvShutdown();
  }
  CAM_Module::onApplicationClosed(theApp);
}

/*!
  \brief Compares the contents of the window with the given reference image,
  returns true if they "match" within some tolerance.
*/
/*VSV it seems that this method is obsolete
bool PVGUI_Module::compareView( const QString& ReferenceImage, double Threshold,
                                std::ostream& Output, const QString& TempDirectory )
{
  if ( Implementation )
    return Implementation->Core.compareView( ReferenceImage, Threshold, Output, TempDirectory );
  return false;
}
*/
QString PVGUI_Module::engineIOR() const
{
  CORBA::String_var anIOR = PARAVIS::GetParavisGen(this)->GetIOR();
  return QString(anIOR.in());
}


/*!
  \brief Open file of format supported by ParaView
*/
void PVGUI_Module::openFile(const char* theName)
{
  QStringList aFiles;
  aFiles<<theName;
  pqLoadDataReaction::loadData(aFiles);
}

/*!
  \brief Returns trace string
*/
QString PVGUI_Module::getTraceString()
{
  pqPythonDialog* pyDiag = 0;
  pqPythonManager* manager = qobject_cast<pqPythonManager*>(
                             pqApplicationCore::instance()->manager("PYTHON_MANAGER"));
  if (manager)  {
    pyDiag = manager->pythonShellDialog();
  }

  QString traceString;
  if (pyDiag) {
    pyDiag->runString("try:\n"
                      "  from paraview import smtrace\n"
                      "  __smtraceString = smtrace.get_trace_string()\n"
                      "except:\n"
                      "  __smtraceString = str()\n"
                      "  raise RuntimeError('could not import paraview.smtrace')\n");
    pyDiag->shell()->makeCurrent();
    PyObject* main_module = PyImport_AddModule((char*)"__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* string_object = PyDict_GetItemString(global_dict, "__smtraceString");
    char* string_ptr = PyString_AsString(string_object);
    if (string_ptr) {
      traceString = string_ptr;
    }
    pyDiag->shell()->releaseControl();
  }
  return traceString;
}

/*!
  \brief Saves trace string to disk file
*/
void PVGUI_Module::saveTrace(const char* theName)
{
  //save_trace(theName);
  QFile file(theName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    MESSAGE( "Could not open file:" << theName );
    return;
  }
  QTextStream out(&file);
  out << getTraceString();
  file.close();
}

/*!
  \brief Saves ParaView state to a disk file
*/
void PVGUI_Module::saveParaviewState(const char* theFileName)
{
  Implementation->Core->saveState(theFileName);
}

/*!
  \brief Restores ParaView state from a disk file
*/
void PVGUI_Module::loadParaviewState(const char* theFileName)
{
  Implementation->Core->loadState(theFileName, getActiveServer());
}

/*!
  \brief Imports MED data from VISU module by data entry
*/
void PVGUI_Module::onImportFromVisu(QString theEntry)
{
#ifdef WITH_VISU
  SUIT_OverrideCursor aWaitCursor;

  // get active study
  SalomeApp_Study* activeStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  if(!activeStudy) return;

  // get SALOMEDS client study 
  _PTR(Study) aStudy = activeStudy->studyDS();
  if(!aStudy) return;

  // find VISU component in a study
  _PTR(SComponent) aVisuComp = aStudy->FindComponent( "VISU" );
  if(!aVisuComp) return;

  // get SObject client by entry
  _PTR(SObject) aSObj = aStudy->FindObjectID(qPrintable(theEntry));
  if (!aSObj) return;

  // get CORBA SObject
  SALOMEDS_SObject* aSObject = _CAST(SObject, aSObj);
  if ( !aSObject ) return;

  // load VISU engine
  SALOME_NamingService* aNamingService = SalomeApp_Application::namingService();
  SALOME_LifeCycleCORBA aLCC(aNamingService);

  Engines::Component_var aComponent = aLCC.FindOrLoad_Component("FactoryServer","VISU");
  VISU::VISU_Gen_var aVISU = VISU::VISU_Gen::_narrow(aComponent);
  if(CORBA::is_nil(aVISU)) return;

  _PTR(StudyBuilder) aStudyBuilder = aStudy->NewBuilder();
  aStudyBuilder->LoadWith( aVisuComp, SalomeApp_Application::orb()->object_to_string(aVISU) );

  // set current study to VISU engine
  //aVISU->SetCurrentStudy(aStudyVar);

  // get VISU result object
  CORBA::Object_var aResultObject = aSObject->GetObject();
  if (CORBA::is_nil(aResultObject)) return;
  VISU::Result_var aResult = VISU::Result::_narrow( aResultObject );
  if (CORBA::is_nil(aResult)) return;

  // export VISU result to the MED file
  std::string aTmpDir = SALOMEDS_Tool::GetTmpDir();
  std::string aFileName = aSObject->GetName();
  std::string aFilePath = aTmpDir + aFileName;

  if (aResult->ExportMED(aFilePath.c_str())) {
    openFile(aFilePath.c_str());
    myTemporaryFiles.append(QString(aFilePath.c_str()));
  }
#else
  MESSAGE("Visu module is not found.");
#endif
}

/*!
  \brief Deletes temporary files created during import operation from VISU
*/
void PVGUI_Module::deleteTemporaryFiles()
{
  foreach(QString aFile, myTemporaryFiles) {
    if (QFile::exists(aFile)) {
      QFile::remove(aFile);
    }
  }
}


/*!
  \brief Returns current active ParaView server
*/
pqServer* PVGUI_Module::getActiveServer()
{
  return Implementation->Core->getActiveServer();
}


/*!
  \brief Creates PARAVIS preference pane 
*/
void PVGUI_Module::createPreferences()
{
  int TraceTab = addPreference( tr( "TIT_TRACE" ) );
  addPreference( tr( "PREF_STOP_TRACE" ), TraceTab, LightApp_Preferences::Bool, "PARAVIS", "stop_trace");

  int aSaveType = addPreference(tr( "PREF_SAVE_TYPE_LBL" ), TraceTab,
                                LightApp_Preferences::Selector,
                                "PARAVIS", "savestate_type");
  QList<QVariant> aIndices;
  QStringList aStrings;
  aIndices<<0<<1<<2;
  aStrings<<tr("PREF_SAVE_TYPE_0");
  aStrings<<tr("PREF_SAVE_TYPE_1");
  aStrings<<tr("PREF_SAVE_TYPE_2");
  setPreferenceProperty(aSaveType, "strings", aStrings);
  setPreferenceProperty(aSaveType, "indexes", aIndices);
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
