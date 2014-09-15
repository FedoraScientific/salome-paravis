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

#include "PVGUI_ViewModel.h"
#include "PVGUI_ViewWindow.h"
#include "PVGUI_OutputWindowAdapter.h"
#include "utilities.h"

#include <SalomeApp_Application.h>

#include <pqPVApplicationCore.h>
#include <pqTabbedMultiViewWidget.h>

//---------- Static init -----------------
pqPVApplicationCore* PVGUI_Viewer::MyCoreApp = 0;
PARAVIS_ORB::PARAVIS_Gen_var PVGUI_Viewer::MyEngine;

//----------------------------------------
PVGUI_Viewer::PVGUI_Viewer()
:SUIT_ViewModel() 
{
  MESSAGE("PVGUI_Viewer: creating view ...");
}

pqPVApplicationCore* PVGUI_Viewer::GetPVApplication()
{
  if(!MyCoreApp)
    pvInit();
  return MyCoreApp;
}

/*!
  \brief Static method, performs initialization of ParaView session.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVGUI_Viewer::pvInit()
{
  //  if ( !pqImplementation::Core ){
  if ( ! MyCoreApp) {
    // Obtain command-line arguments
    int argc = 0;
    char** argv = 0;
    QString aOptions = getenv("PARAVIS_OPTIONS");
    QStringList aOptList = aOptions.split(":", QString::SkipEmptyParts);
    argv = new char*[aOptList.size() + 1];
    QStringList args = QApplication::arguments();
    argv[0] = (args.size() > 0)? strdup(args[0].toLatin1().constData()) : strdup("paravis");
    argc++;

    foreach (QString aStr, aOptList) {
      argv[argc] = strdup( aStr.toLatin1().constData() );
      argc++;
    }
    MyCoreApp = new pqPVApplicationCore (argc, argv);
    if (MyCoreApp->getOptions()->GetHelpSelected() ||
        MyCoreApp->getOptions()->GetUnknownArgument() ||
        MyCoreApp->getOptions()->GetErrorMessage() ||
        MyCoreApp->getOptions()->GetTellVersion()) {
      return false;
      }

    /* VTN: Looks like trash. For porting see branded_paraview_initializer.cxx.in
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
    */
    // End of Initializer code

    vtkOutputWindow::SetInstance(PVGUI_OutputWindowAdapter::New());

    new pqTabbedMultiViewWidget(); // it registers as "MULTIVIEW_WIDGET" on creation

    for (int i = 0; i < argc; i++)
      free(argv[i]);
    delete[] argv;
  }

  return true;
}


PARAVIS_ORB::PARAVIS_Gen_var PVGUI_Viewer::GetEngine()
{
  // initialize PARAVIS module engine (load, if necessary)
  if ( CORBA::is_nil( myEngine ) ) {
    Engines::EngineComponent_var comp =
        SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "PARAVIS" );
    myEngine = PARAVIS_ORB::PARAVIS_Gen::_narrow( comp );
  }
  return myEngine;
}

/*!
  Create new instance of view window on desktop \a theDesktop.
  \retval SUIT_ViewWindow* - created view window pointer.
*/
SUIT_ViewWindow* PVGUI_Viewer::createView(SUIT_Desktop* theDesktop)
{
  PVGUI_ViewWindow* aPVView = new PVGUI_ViewWindow(theDesktop, this);
  return aPVView;
}

bool PVGUI_Viewer::connectToExternalPVServer()
{
  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      // Already connected to an external server, do nothing
      MESSAGE("connectToExternalPVServer(): Already connected to an external PVServer, won't reconnect.");
      return false;
    }

  std::stringstream msg;

  // Try to connect to the external PVServer - gives priority to an externally specified URL:
  QString serverUrlEnv = getenv("PARAVIS_PVSERVER_URL");
  std::string serverUrl;
  if (!serverUrlEnv.isEmpty())
    serverUrl = serverUrlEnv.toStdString();
  else
    {
      // Get the URL from the engine (possibly starting the pvserver)
      CORBA::String_var url = GetEngine()->FindOrStartPVServer(0);  // take the first free port
      serverUrl = (char *)url;
    }

  msg << "connectToExternalPVServer(): Trying to connect to the external PVServer '" << serverUrl << "' ...";
  MESSAGE(msg.str());

  if (!pqServerConnectReaction::connectToServer(pqServerResource(serverUrl.c_str())))
    {
      std::stringstream msg2;
      msg2 << "Error while connecting to the requested pvserver '" << serverUrl;
      msg2 << "'. Might use default built-in connection instead!" << std::endl;
      qWarning(msg2.str().c_str());  // will go to the ParaView console (see ParavisMessageOutput below)
      SUIT_MessageBox::warning( getApp()->desktop(),
                                QString("Error connecting to PVServer"), QString(msg2.str().c_str()));
      return false;
    }
  else
    MESSAGE("connectToExternalPVServer(): Connected!");
  return true;
}
