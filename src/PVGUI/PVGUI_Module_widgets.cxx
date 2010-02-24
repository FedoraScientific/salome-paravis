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
// File   : PVGUI_Module_MenuActions.cxx
// Author : Margarita KARPUNINA
//

#include "PVGUI_Module_impl.h"

#include <QtxActionToolMgr.h>
#include <LightApp_Application.h>
#include <SUIT_Desktop.h>

#include <QAction>
#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QShortcut>

#include <pqAnimationViewWidget.h> //VSV

#include <pqApplicationCore.h>
#include <pqComparativeVisPanel.h>
#include <pqObjectInspectorWidget.h>
#include <pqPipelineBrowserWidget.h>
#include <pqProxyTabWidget.h>
#include <pqSettings.h>
#include <pqDataInformationWidget.h>
#include <pqPVAnimationWidget.h>
#include <pqSelectionInspectorPanel.h>
#include <pqProgressWidget.h>

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
#include <pqSpreadSheetVisibilityBehavior.h>
#include <pqStandardViewModules.h>
#include <pqUndoRedoBehavior.h>
#include <pqViewFrameActionsBehavior.h>
#include <pqParaViewMenuBuilders.h>

/*!
  \brief Create dock widgets for ParaView widgets such as object inspector, pipeline browser, etc.
  ParaView pqMainWIndowCore class is fully responsible for these dock widgets' contents.
*/
void PVGUI_Module::setupDockWidgets()
{
  SUIT_Desktop* desk = application()->desktop();

  // Pipeline
  QDockWidget* pipelineBrowserDock = new QDockWidget( tr( "TTL_PIPELINE_BROWSER" ), desk );
  pipelineBrowserDock->setObjectName("pipelineBrowserDock");
  pipelineBrowserDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, pipelineBrowserDock );
  pqPipelineBrowserWidget* browser = new pqPipelineBrowserWidget(pipelineBrowserDock);
  pqParaViewMenuBuilders::buildPipelineBrowserContextMenu(*browser);
  pipelineBrowserDock->setWidget(browser);
  myDockWidgets.append(pipelineBrowserDock);

  //Object inspector
  QDockWidget* objectInspectorDock = new QDockWidget( tr( "TTL_OBJECT_INSPECTOR" ), desk );
  objectInspectorDock->setObjectName("objectInspectorDock");
  objectInspectorDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, objectInspectorDock );
  pqProxyTabWidget* proxyTab = new pqProxyTabWidget(objectInspectorDock);
  proxyTab->setShowOnAccept(true);
  objectInspectorDock->setWidget(proxyTab);
  connect( proxyTab->getObjectInspector(), SIGNAL( helpRequested(QString) ),
          this, SLOT( showHelpForProxy(QString) ) );
  //connect( proxyTab->getObjectInspector(), SIGNAL( preaccept() ), this, SLOT( onPreAccept() ) );
  //connect( proxyTab->getObjectInspector(), SIGNAL( postaccept() ), this, SLOT( onPostAccept() ) );
  myDockWidgets.append(objectInspectorDock);

  // Statistic View
  QDockWidget* statisticsViewDock  = new QDockWidget( tr( "TTL_STATISTICS_VIEW" ), desk );
  statisticsViewDock->setObjectName("statisticsViewDock");
  statisticsViewDock->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|
                                      Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::BottomDockWidgetArea, statisticsViewDock );
  pqDataInformationWidget* aStatWidget = new pqDataInformationWidget(statisticsViewDock);
  statisticsViewDock->setWidget(aStatWidget);
  myDockWidgets.append(statisticsViewDock);

  //Animation view
  QDockWidget* animationViewDock     = new QDockWidget( tr( "TTL_ANIMATION_VIEW" ), desk );
  animationViewDock->setObjectName("animationViewDock");
  desk->addDockWidget( Qt::BottomDockWidgetArea, animationViewDock );
  pqPVAnimationWidget* animation_panel = new pqPVAnimationWidget(animationViewDock);
  animationViewDock->setWidget(animation_panel);
  myDockWidgets.append(animationViewDock);

  // Selection view
  QDockWidget* selectionInspectorDock = new QDockWidget( tr( "TTL_SELECTION_INSPECTOR" ), desk );
  selectionInspectorDock->setObjectName("selectionInspectorDock");
  selectionInspectorDock->setAllowedAreas( Qt::AllDockWidgetAreas );
  desk->addDockWidget( Qt::LeftDockWidgetArea, selectionInspectorDock );
  pqSelectionInspectorPanel* aSelInspector = new pqSelectionInspectorPanel(selectionInspectorDock);
  selectionInspectorDock->setWidget(aSelInspector);
  myDockWidgets.append(selectionInspectorDock);

  // Comparative View
  QDockWidget* comparativePanelDock  = new QDockWidget( tr( "TTL_COMPARATIVE_VIEW_INSPECTOR" ), desk );
  comparativePanelDock->setObjectName("comparativePanelDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, comparativePanelDock );
  pqComparativeVisPanel* cv_panel = new pqComparativeVisPanel( comparativePanelDock );
  comparativePanelDock->setWidget(cv_panel);
  myDockWidgets.append(comparativePanelDock);

  // Setup the statusbar ...
  pqProgressWidget* aProgress = new pqProgressWidget(desk->statusBar());
  desk->statusBar()->addPermanentWidget(aProgress);
  
  // Set up the dock window corners to give the vertical docks more room.
  desk->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  
  // Setup the default dock configuration ...
  statisticsViewDock->hide();
  comparativePanelDock->hide();
  animationViewDock->hide();
  selectionInspectorDock->hide();


  // Setup quick-launch shortcuts.
  QShortcut *ctrlSpace = new QShortcut(Qt::CTRL + Qt::Key_Space, desk);
  QObject::connect(ctrlSpace, SIGNAL(activated()), pqApplicationCore::instance(), SLOT(quickLaunch()));
  QShortcut *altSpace = new QShortcut(Qt::ALT + Qt::Key_Space, desk);
  QObject::connect(altSpace, SIGNAL(activated()), pqApplicationCore::instance(), SLOT(quickLaunch()));

}

/*!
  \brief Save states of dockable ParaView widgets.
*/
void PVGUI_Module::saveDockWidgetsState()
{
  SUIT_Desktop* desk = application()->desktop();

  // Save the state of the window ...
  pqApplicationCore::instance()->settings()->saveState(*desk, "MainWindow");

  for (int i = 0; i < myDockWidgets.size(); ++i)
    myDockWidgets.at(i)->setParent(0);
}

/*!
  \brief Restore states of dockable ParaView widgets.
*/
void PVGUI_Module::restoreDockWidgetsState()
{
  SUIT_Desktop* desk = application()->desktop();

  for (int i = 0; i < myDockWidgets.size(); ++i)
    myDockWidgets.at(i)->setParent(desk);

  // Restore the state of the window ...
  pqApplicationCore::instance()->settings()->restoreState("MainWindow", *desk);
}
