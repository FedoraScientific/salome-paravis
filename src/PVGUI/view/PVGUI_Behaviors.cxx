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
// Author: Adrien Bruneton (CEA)

#include "PVGUI_Behaviors.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Module.h>

#include <pqInterfaceTracker.h>
#include <pqApplicationCore.h>
#include <pqPluginManager.h>
#include <pqStandardViewModules.h>
#include <pqStandardPropertyWidgetInterface.h>

#include <pqAlwaysConnectedBehavior.h>
#include <pqAutoLoadPluginXMLBehavior.h>
#include <pqCommandLineOptionsBehavior.h>
#include <pqCrashRecoveryBehavior.h>
#include <pqDataTimeStepBehavior.h>
#include <pqDefaultViewBehavior.h>
#include <pqDeleteBehavior.h>
#include <pqObjectPickingBehavior.h>
#include <pqPersistentMainWindowStateBehavior.h>
#include <pqPipelineContextMenuBehavior.h>
#include <pqPluginActionGroupBehavior.h>
#include <pqPluginDockWidgetsBehavior.h>
#include <pqPVNewSourceBehavior.h>
#include <pqSpreadSheetVisibilityBehavior.h>
#include <pqUndoRedoBehavior.h>
#include <pqViewFrameActionsBehavior.h>
#include <pqViewStreamingBehavior.h>
#include <pqCollaborationBehavior.h>

bool PVGUI_Behaviors::hasMinimalInstanciated = false;

PVGUI_Behaviors::PVGUI_Behaviors(SalomeApp_Module * parent)
  : QObject(static_cast<QObject *>(parent))
{
}

/**! Instanciate minimal ParaView behaviors needed when using an instance of PVViewer.
 * This method should be updated at each new version of ParaView with what is found in
 *    Qt/ApplicationComponents/pqParaViewBehaviors.cxx
 */
void PVGUI_Behaviors::instanciateMinimalBehaviors(SUIT_Desktop * desk)
{
  hasMinimalInstanciated = true;

  // Register ParaView interfaces.
  pqInterfaceTracker* pgm = pqApplicationCore::instance()->interfaceTracker();

  // * adds support for standard paraview views.
  pgm->addInterface(new pqStandardViewModules(pgm));
  pgm->addInterface(new pqStandardPropertyWidgetInterface(pgm));

  // Load plugins distributed with application.
  pqApplicationCore::instance()->loadDistributedPlugins();

  new pqViewFrameActionsBehavior(this);     // button above the view port controlling selection and camera undos
  new pqDefaultViewBehavior(this);  // shows a 3D view as soon as a server connection is made
  new pqAlwaysConnectedBehavior(this);  // client always connected to a server
  new pqPVNewSourceBehavior(this);  // new source is made active, ...
  new pqAutoLoadPluginXMLBehavior(this);  // auto load plugins
}

/**! Instanciate usual ParaView behaviors.
 * This method should be updated at each new version of ParaView with what is found in
 *    Qt/ApplicationComponents/pqParaViewBehaviors.cxx
 */
void PVGUI_Behaviors::instanciateAllBehaviors(SUIT_Desktop * desk)
{
  //    "new pqParaViewBehaviors(anApp->desktop(), this);"
  // -> (which loads all standard ParaView behaviors at once) has to be replaced in order to
  // exclude using of pqQtMessageHandlerBehaviour

  // Define application behaviors.
  if (!hasMinimalInstanciated)
    instanciateMinimalBehaviors(desk);
  //new pqQtMessageHandlerBehavior(this);   // THIS ONE TO EXCLUDE !! see comment above
  new pqDataTimeStepBehavior(this);
  new pqSpreadSheetVisibilityBehavior(this);
  new pqPipelineContextMenuBehavior(this);
  new pqObjectPickingBehavior(this); // NEW in 4.1
  new pqDeleteBehavior(this);
  new pqUndoRedoBehavior(this);
  new pqCrashRecoveryBehavior(this);
  new pqPluginDockWidgetsBehavior(desk);
  //new pqVerifyRequiredPluginBehavior(this);
  new pqPluginActionGroupBehavior(desk);
  //new pqFixPathsInStateFilesBehavior(this);
  new pqCommandLineOptionsBehavior(this);
  new pqPersistentMainWindowStateBehavior(desk);
  new pqObjectPickingBehavior(desk);
  new pqCollaborationBehavior(this);
  //new pqMultiServerBehavior(this);
  new pqViewStreamingBehavior(this);

}
