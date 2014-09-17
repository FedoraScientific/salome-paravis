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
#ifndef PVViewer_VIEWMANAGER_H
#define PVViewer_VIEWMANAGER_H

#include <SUIT_ViewManager.h>
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(PARAVIS_Gen)

class SUIT_Desktop;
class SUIT_Study;
class SUIT_ViewWindow;
class pqTabbedMultiViewWidget;
class pqPVApplicationCore;
class PVViewer_Behaviors;
class pqPropertiesPanel;

class PVViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  PVViewer_ViewManager( SUIT_Study*, SUIT_Desktop* );
  ~PVViewer_ViewManager() {}

  static pqPVApplicationCore * GetPVApplication();
  static PARAVIS_ORB::PARAVIS_Gen_var GetEngine();

  //! Initialize ParaView if not yet done (once per session)
  static bool   ParaviewInitApp(SUIT_Desktop* aDesktop);
  static void   ParaviewInitBehaviors(bool fullSetup=false, SUIT_Desktop* aDesktop=0);
  static void   ParaviewLoadConfigurations();
  static void   ParaviewCleanup();

  //! Connect to the external PVServer, using the PARAVIS engine to launch it if it is not
  //! already up.
  static bool   ConnectToExternalPVServer(SUIT_Desktop* aDesktop);

public slots:
  void onPVViewCreated(SUIT_ViewWindow*);

private:
  static pqPVApplicationCore* MyCoreApp;
  static PARAVIS_ORB::PARAVIS_Gen_var MyEngine;
  static bool ConfigLoaded;
  static PVViewer_Behaviors * ParaviewBehaviors;
};

#endif
