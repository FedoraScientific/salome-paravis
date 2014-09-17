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
#include <PVViewer_ViewManager.h>
#include <PVViewer_ViewModel.h>
#include <PVViewer_ViewWindow.h>
#include <PVViewer_Behaviors.h>
#include <PVViewer_LogWindowAdapter.h>
#include <utilities.h>
#include <SalomeApp_Application.h>
#include <SALOMEconfig.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SUIT_MessageBox.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <PyInterp_Interp.h>
#include <PyConsole_Interp.h>
#include <PyConsole_Console.h>

#include <QApplication>
#include <QStringList>
#include <string>

#include <pqOptions.h>
#include <pqServer.h>
#include <pqSettings.h>
#include <pqServerDisconnectReaction.h>
#include <pqPVApplicationCore.h>
#include <pqTabbedMultiViewWidget.h>
#include <pqActiveObjects.h>
#include <pqServerConnectReaction.h>

//---------- Static init -----------------
pqPVApplicationCore* PVViewer_ViewManager::MyCoreApp = 0;
PARAVIS_ORB::PARAVIS_Gen_var PVViewer_ViewManager::MyEngine;


/*!
  Constructor
*/
PVViewer_ViewManager::PVViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* desk )
: SUIT_ViewManager( study, desk, new PVViewer_Viewer() )
{
  MESSAGE("PARAVIS - view manager created ...")
  setTitle( tr( "PARAVIEW_VIEW_TITLE" ) );
}

pqPVApplicationCore * PVViewer_ViewManager::GetPVApplication()
{
  return MyCoreApp;
}

/*!
  \brief Static method, performs initialization of ParaView session.
  \param fullSetup whether to instanciate all behaviors or just the minimal ones.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVViewer_ViewManager::ParaviewInitApp(SUIT_Desktop * aDesktop)
{
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

      // Direct VTK log messages to our SALOME window - TODO: review this
      vtkOutputWindow::SetInstance(PVViewer_LogWindowAdapter::New());

      new pqTabbedMultiViewWidget(); // registers a "MULTIVIEW_WIDGET" on creation

      // At this stage, the pqPythonManager has been initialized, i.e. the current process has
      // activated the embedded Python interpreter. "paraview" package has also been imported once already.
      // Make sure the current process executes paraview's Python command with the "fromGUI" flag.
      // This is used in pvsimple.py to avoid reconnecting the GUI thread to the pvserver (when
      // user types "import pvsimple" in SALOME's console).
      SalomeApp_Application* app =
                  dynamic_cast< SalomeApp_Application* >(SUIT_Session::session()->activeApplication());
      PyConsole_Interp* pyInterp = app->pythonConsole()->getInterp();
      {
        PyLockWrapper aGil;
        std::string cmd = "import paraview; paraview.fromGUI = True;";
        pyInterp->run(cmd.c_str());
      }

      for (int i = 0; i < argc; i++)
        free(argv[i]);
      delete[] argv;
  }

  return true;
}

void PVViewer_ViewManager::ParaviewInitBehaviors(bool fullSetup, SUIT_Desktop* aDesktop)
{
  PVViewer_Behaviors * behav = new PVViewer_Behaviors(aDesktop);
  if(fullSetup)
    behav->instanciateAllBehaviors(aDesktop);
  else
    behav->instanciateMinimalBehaviors(aDesktop);
}

void PVViewer_ViewManager::ParaviewCleanup()
{
  // Disconnect from server
  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      MESSAGE("~PVViewer_Module(): Disconnecting from remote server ...");
      pqServerDisconnectReaction::disconnectFromServer();
    }

  pqApplicationCore::instance()->settings()->sync();

  pqPVApplicationCore * app = GetPVApplication();
  // Schedule destruction of PVApplication singleton:
  if (app)
    app->deleteLater();
}

PARAVIS_ORB::PARAVIS_Gen_var PVViewer_ViewManager::GetEngine()
{
  // initialize PARAVIS module engine (load, if necessary)
  if ( CORBA::is_nil( MyEngine ) ) {
    Engines::EngineComponent_var comp =
        SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "PARAVIS" );
    MyEngine = PARAVIS_ORB::PARAVIS_Gen::_narrow( comp );
  }
  return MyEngine;
}

bool PVViewer_ViewManager::ConnectToExternalPVServer(SUIT_Desktop* aDesktop)
{
  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      // Already connected to an external server, do nothing
      MESSAGE("connectToExternalPVServer(): Already connected to an external PVServer, won't reconnect.");
      return false;
    }

  if (GetEngine()->GetGUIConnected())
    {
      // Should never be there as the above should already tell us that we are connected.
      std::stringstream msg2;
      msg2 << "Internal error while connecting to the pvserver.";
      msg2 << "ParaView doesn't see a connection, but PARAVIS engine tells us there is already one!" << std::endl;
      qWarning(msg2.str().c_str());  // will go to the ParaView console (see ParavisMessageOutput below)
      SUIT_MessageBox::warning( aDesktop,
                                      QString("Error connecting to PVServer"), QString(msg2.str().c_str()));
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
      SUIT_MessageBox::warning( aDesktop,
                                QString("Error connecting to PVServer"), QString(msg2.str().c_str()));
      return false;
    }
  else
    {
      MESSAGE("connectToExternalPVServer(): Connected!");
      GetEngine()->SetGUIConnected(true);
    }
  return true;
}
