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

#include <pqAnimationPanel.h>
#include <pqApplicationCore.h>
#include <pqComparativeVisPanel.h>
#include <pqMainWindowCore.h>
#include <pqObjectInspectorWidget.h>
#include <pqPipelineBrowser.h>
#include <pqPipelineBrowserContextMenu.h>
#include <pqPipelineMenu.h>
#include <pqProxyTabWidget.h>
#include <pqSettings.h>

/*!
  \brief Create dock widgets for ParaView widgets such as object inspector, pipeline browser, etc.
  ParaView pqMainWIndowCore class is fully responsible for these dock widgets' contents.
*/
void PVGUI_Module::setupDockWidgets()
{
  SUIT_Desktop* desk = application()->desktop();

  // See ParaView src/Applications/Client/MainWindow.cxx
  QDockWidget* pipelineBrowserDock = new QDockWidget( tr( "TTL_PIPELINE_BROWSER" ), desk );
  pipelineBrowserDock->setObjectName("pipelineBrowserDock");
  pipelineBrowserDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, pipelineBrowserDock );
  Implementation->Core.setupPipelineBrowser( pipelineBrowserDock );
  pqPipelineBrowser *browser = Implementation->Core.pipelineBrowser();
  Implementation->Core.pipelineMenu().setModels(browser->getModel(), browser->getSelectionModel());
  myDockWidgets.append(pipelineBrowserDock);

  QDockWidget* objectInspectorDock = new QDockWidget( tr( "TTL_OBJECT_INSPECTOR" ), desk );
  objectInspectorDock->setObjectName("objectInspectorDock");
  objectInspectorDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, objectInspectorDock );
  pqProxyTabWidget* const proxyTab = Implementation->Core.setupProxyTabWidget( objectInspectorDock );
  connect( proxyTab->getObjectInspector(), SIGNAL( helpRequested(QString) ),
	   this,                           SLOT( showHelpForProxy(QString) ) );
  connect( proxyTab->getObjectInspector(), SIGNAL( preaccept() ),
	   this,                           SLOT( onPreAccept() ) );
  connect( proxyTab->getObjectInspector(), SIGNAL( postaccept() ),
	   this,                           SLOT( onPostAccept() ) );
  myDockWidgets.append(objectInspectorDock);
  
  QDockWidget* statisticsViewDock  = new QDockWidget( tr( "TTL_STATISTICS_VIEW" ), desk );
  statisticsViewDock->setObjectName("statisticsViewDock");
  statisticsViewDock->setAllowedAreas( Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::BottomDockWidgetArea, statisticsViewDock );
  Implementation->Core.setupStatisticsView( statisticsViewDock );
  myDockWidgets.append(statisticsViewDock);

  QDockWidget* animationPanelDock  = new QDockWidget( tr( "TTL_ANIMATION_INSPECTOR" ), desk );
  animationPanelDock->setObjectName("animationPanelDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, animationPanelDock );
  pqAnimationPanel* animation_panel = Implementation->Core.setupAnimationPanel( animationPanelDock );
  
  int aToolId = createTool( tr("TOOL_CURRENT_TIME_CONTROLS") );
  QToolBar* aTB = toolMgr()->toolBar( aToolId );
  animation_panel->setCurrentTimeToolbar( aTB );
  QList<QAction*> anActns = aTB->actions();
  for (int i = 0; i < anActns.size(); ++i)
    createTool( anActns.at(i), aToolId );

  myDockWidgets.append(animationPanelDock);

  QDockWidget* lookmarkBrowserDock = new QDockWidget( tr( "TTL_LOOKMARK_BROWSER" ), desk );
  lookmarkBrowserDock->setObjectName("lookmarkBrowserDock");
  QSizePolicy sp( QSizePolicy::Preferred, QSizePolicy::Preferred );
  sp.setHorizontalStretch( 0 );
  sp.setVerticalStretch( 0 );
  lookmarkBrowserDock->setSizePolicy( sp );
  lookmarkBrowserDock->setFloating( false );
  desk->addDockWidget( Qt::RightDockWidgetArea, lookmarkBrowserDock );
  Implementation->Core.setupLookmarkBrowser( lookmarkBrowserDock );
  myDockWidgets.append(lookmarkBrowserDock);

  QDockWidget* lookmarkInspectorDock = new QDockWidget( tr( "TTL_LOOKMARK_INSPECTOR" ), desk );
  lookmarkInspectorDock->setObjectName("lookmarkInspectorDock");
  lookmarkInspectorDock->setAllowedAreas( Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::RightDockWidgetArea, lookmarkInspectorDock );
  Implementation->Core.setupLookmarkInspector( lookmarkInspectorDock );
  myDockWidgets.append(lookmarkInspectorDock);

  QDockWidget* comparativePanelDock  = new QDockWidget( tr( "TTL_COMPARATIVE_VIEW_INSPECTOR" ), desk );
  comparativePanelDock->setObjectName("comparativePanelDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, comparativePanelDock );
  pqComparativeVisPanel* cv_panel    = new pqComparativeVisPanel( comparativePanelDock );
  comparativePanelDock->setWidget(cv_panel);
  myDockWidgets.append(comparativePanelDock);

  QDockWidget* animationViewDock     = new QDockWidget( tr( "TTL_ANIMATION_VIEW" ), desk );
  animationViewDock->setObjectName("animationViewDock");
  desk->addDockWidget( Qt::BottomDockWidgetArea, animationViewDock );
  Implementation->Core.setupAnimationView( animationViewDock );
  myDockWidgets.append(animationViewDock);

  QDockWidget* selectionInspectorDock = new QDockWidget( tr( "TTL_SELECTION_INSPECTOR" ), desk );
  selectionInspectorDock->setObjectName("selectionInspectorDock");
  selectionInspectorDock->setAllowedAreas( Qt::AllDockWidgetAreas );
  desk->addDockWidget( Qt::LeftDockWidgetArea, selectionInspectorDock );
  Implementation->Core.setupSelectionInspector( selectionInspectorDock );
  myDockWidgets.append(selectionInspectorDock);

  // Setup the statusbar ...
  Implementation->Core.setupProgressBar( desk->statusBar() );

  // Set up the dock window corners to give the vertical docks more room.
  desk->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  // Setup the default dock configuration ...
  lookmarkBrowserDock->hide();
  lookmarkInspectorDock->hide();
  statisticsViewDock->hide();
  animationPanelDock->hide();
  comparativePanelDock->hide();
  animationViewDock->hide();
  selectionInspectorDock->hide();
}

/*!
  \brief Create dock widgets context menus.
*/
void PVGUI_Module::setupDockWidgetsContextMenu()
{
  // Pipeline menu
  Implementation->Core.pipelineMenu().setMenuAction( pqPipelineMenu::ChangeInputAction,
						     action(ChangeInputId) );
  Implementation->Core.pipelineMenu().setMenuAction( pqPipelineMenu::DeleteAction,
						     action(DeleteId) );

  // Pipeline Browser menu
  pqPipelineBrowser *browser = Implementation->Core.pipelineBrowser();
  pqPipelineBrowserContextMenu *browserMenu = new pqPipelineBrowserContextMenu(browser);

  browserMenu->setMenuAction(action(OpenFileId));
  if ( action(OpenFileId)->text().compare(tr("MEN_OPEN")) == 0 )
    action(OpenFileId)->setText(tr("MEN_OPEN_FILE"));

  browserMenu->setMenuAction(action(ChangeInputId));
  browserMenu->setMenuAction(action(DeleteId));
  browserMenu->setMenuAction(action(CreateCustomFilterId));
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
