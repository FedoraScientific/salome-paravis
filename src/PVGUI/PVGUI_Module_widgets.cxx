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

#include <pqAnimationPanel.h>
#include <pqComparativeVisPanel.h>
#include <pqMainWindowCore.h>
#include <pqObjectInspectorWidget.h>
#include <pqPipelineBrowser.h>
#include <pqPipelineBrowserContextMenu.h>
#include <pqPipelineMenu.h>
#include <pqProxyTabWidget.h>

/*!
  \brief Create dock widgets for ParaView widgets such as object inspector, pipeline browser, etc.
  ParaView pqMainWIndowCore class is fully responsible for these dock widgets' contents.
*/
void PVGUI_Module::setupDockWidgets()
{
  SUIT_Desktop* desk = application()->desktop();

  // See ParaView src/Applications/Client/MainWindow.cxx
  QDockWidget* pipelineBrowserDock = new QDockWidget( tr( "Pipeline Browser" ), desk );
  pipelineBrowserDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, pipelineBrowserDock );
  Implementation->Core.setupPipelineBrowser( pipelineBrowserDock );
  pqPipelineBrowser *browser = Implementation->Core.pipelineBrowser();
  Implementation->Core.pipelineMenu().setModels(browser->getModel(), browser->getSelectionModel());

  QDockWidget* objectInspectorDock = new QDockWidget( tr( "Object Inspector" ), desk );
  objectInspectorDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, objectInspectorDock );
  pqProxyTabWidget* const proxyTab = Implementation->Core.setupProxyTabWidget( objectInspectorDock );
  connect( proxyTab->getObjectInspector(), SIGNAL( helpRequested(QString) ),
	   this,                           SLOT( showHelpForProxy(QString) ) );
  connect( proxyTab->getObjectInspector(), SIGNAL( preaccept() ),
	   this,                           SLOT( onPreAccept() ) );
  connect( proxyTab->getObjectInspector(), SIGNAL( postaccept() ),
	   this,                           SLOT( onPostAccept() ) );
  
  QDockWidget* statisticsViewDock  = new QDockWidget( tr( "Statistics View" ), desk );
  statisticsViewDock->setAllowedAreas( Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::BottomDockWidgetArea, statisticsViewDock );
  Implementation->Core.setupStatisticsView( statisticsViewDock );

  QDockWidget* animationPanelDock  = new QDockWidget( tr( "Animation Inspector" ), desk );
  desk->addDockWidget( Qt::LeftDockWidgetArea, animationPanelDock );
  pqAnimationPanel* animation_panel = Implementation->Core.setupAnimationPanel( animationPanelDock );
  animation_panel->setCurrentTimeToolbar(toolMgr()->toolBar(createTool( tr("TOOL_CURRENT_TIME_CONTROLS") )));

  QDockWidget* lookmarkBrowserDock = new QDockWidget( tr( "Lookmark Browser" ), desk );
  QSizePolicy sp( QSizePolicy::Preferred, QSizePolicy::Preferred );
  sp.setHorizontalStretch( 0 );
  sp.setVerticalStretch( 0 );
  lookmarkBrowserDock->setSizePolicy( sp );
  lookmarkBrowserDock->setFloating( false );
  desk->addDockWidget( Qt::RightDockWidgetArea, lookmarkBrowserDock );
  Implementation->Core.setupLookmarkBrowser( lookmarkBrowserDock );

  QDockWidget* lookmarkInspectorDock = new QDockWidget( tr( "Lookmark Inspector" ), desk );
  lookmarkInspectorDock->setAllowedAreas( Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::RightDockWidgetArea, lookmarkInspectorDock );
  Implementation->Core.setupLookmarkInspector( lookmarkInspectorDock );

  QDockWidget* comparativePanelDock  = new QDockWidget( tr( "Comparative View Inspector" ), desk );
  desk->addDockWidget( Qt::LeftDockWidgetArea, comparativePanelDock );
  pqComparativeVisPanel* cv_panel    = new pqComparativeVisPanel( comparativePanelDock );
  comparativePanelDock->setWidget(cv_panel);

  QDockWidget* animationViewDock     = new QDockWidget( tr( "Animation View" ), desk );
  desk->addDockWidget( Qt::BottomDockWidgetArea, animationViewDock );
  Implementation->Core.setupAnimationView( animationViewDock );

  QDockWidget* selectionInspectorDock = new QDockWidget( tr( "Selection Inspector" ), desk );
  selectionInspectorDock->setAllowedAreas( Qt::AllDockWidgetAreas );
  desk->addDockWidget( Qt::LeftDockWidgetArea, selectionInspectorDock );
  Implementation->Core.setupSelectionInspector( selectionInspectorDock );

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
