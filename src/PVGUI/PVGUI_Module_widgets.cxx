// PARAVIS : ParaView wrapper SALOME module
//
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
// File   : PVGUI_Module_MenuActions.cxx
// Author : Margarita KARPUNINA
//

#include "PVGUI_Module.h"
#include "PVViewer_ViewManager.h"
#include "PVViewer_GUIElements.h"

#include <QtxActionToolMgr.h>
#include <LightApp_Application.h>
#include <SalomeApp_Application.h>   // // should ultimately be a LightApp only
#include <SUIT_Desktop.h>

#include <QApplication>
#include <QAction>
#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QShortcut>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QShowEvent>
#include <QToolButton>

#include <pqAnimationViewWidget.h> 
#include <pqAnimationWidget.h> 

#include <pqApplicationCore.h>
#include <pqComparativeVisPanel.h>
#include <pqPipelineBrowserWidget.h>
#include <pqProxyInformationWidget.h>
#include <pqSettings.h>
#include <pqDataInformationWidget.h>
#include <pqPVAnimationWidget.h>
#include <pqFindDataSelectionDisplayFrame.h>
#include <pqMultiBlockInspectorPanel.h>
#include <pqProgressWidget.h>
#include <pqProgressManager.h>
//#include <pqDisplayProxyEditorWidget.h>
#include <pqPropertiesPanel.h>

#include <pqApplicationCore.h>
#include <pqPluginManager.h>
#include <pqParaViewMenuBuilders.h>
#include <pqCollaborationPanel.h>
#include <pqMemoryInspectorPanel.h>
#include <pqColorMapEditor.h>
#include <pqDeleteReaction.h>

#include <vtkPVGeneralSettings.h>

class ResizeHelper : public pqPVAnimationWidget
{
  // TEMPORARILY WORKAROUND AROUND PARAVIEW 3.14 BUG:
  // WHEN ANIMATION VIEW IS RESIZED, ITS CONTENTS IS NOT PREPERLY RE-ARRANGED
  // CAUSING SOME CONTROLS TO STAY NON-VISIBLE
  // THIS BUG IS NATURALLY FIXED BY ADDING
  //      this->updateGeometries();
  // TO THE
  //     void pqAnimationWidget::resizeEvent(QResizeEvent* e);
  // BUT THIS CANNOT BE DONE DIRECTLY, SINCE CORRESPONDING API IS NOT PUBLIC
  // THE ONLY WAY TO DO THIS BY SENDING SHOW EVENT TO THE WIDGET

public:
  ResizeHelper( QWidget* parent ) : pqPVAnimationWidget( parent ) {}
protected:
  void resizeEvent(QResizeEvent* e)
  {
    pqAnimationWidget* w = findChild<pqAnimationWidget*>( "pqAnimationWidget" );
    if ( w ) {
      QShowEvent e;
      QApplication::sendEvent( w, &e );
    }
    pqPVAnimationWidget::resizeEvent( e );
  }
};



/*!
  \brief Create dock widgets for ParaView widgets such as object inspector, pipeline browser, etc.
  ParaView pqMainWindowCore class is fully responsible for these dock widgets' contents.
  ==> To update this function, see the reference set up of ParaView in Application/Paraview/ParaviewMainWindow.ui
*/
void PVGUI_Module::setupDockWidgets()
{
  SUIT_Desktop* desk = application()->desktop();
  PVViewer_GUIElements * guiElements = PVViewer_GUIElements::GetInstance(desk);
 
  desk->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  desk->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

  // Pipeline
  QDockWidget* pipelineBrowserDock = new QDockWidget( tr( "TTL_PIPELINE_BROWSER" ), desk );
  pipelineBrowserDock->setObjectName("pipelineBrowserDock");
  pipelineBrowserDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, pipelineBrowserDock );
  pqPipelineBrowserWidget* browser = guiElements->getPipelineBrowserWidget();
  pipelineBrowserDock->setWidget(browser);
  myDockWidgets[pipelineBrowserDock] = true;
  pipelineBrowserDock->hide();

  // PROPERTIES, DISPLAY and VIEW:
  // See ParaViewMainWindow.cxx - those three panels can be separated or grouped.
  pqSettings *settings = pqApplicationCore::instance()->settings();

  //    Properties dock (previously called OBJECT_INSPECTOR)
  QDockWidget* propertiesDock = new QDockWidget( tr( "TTL_OBJECT_INSPECTOR" ), desk );
  propertiesDock->setObjectName("propertiesDock");
  propertiesDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, propertiesDock );

  pqPropertiesPanel* propertiesPanel = guiElements->getPropertiesPanel();
  propertiesDock->setWidget(propertiesPanel);
  connect( propertiesPanel, SIGNAL( helpRequested(const QString&, const QString&) ),  this, SLOT( showHelpForProxy(const QString&, const QString&) ) );
  //            hook delete to pqDeleteReaction.
  QAction* tempDeleteAction = new QAction(this);
  pqDeleteReaction* handler = new pqDeleteReaction(tempDeleteAction);
  handler->connect(propertiesPanel, SIGNAL(deleteRequested(pqPipelineSource*)), SLOT(deleteSource(pqPipelineSource*)));
  myDockWidgets[propertiesDock] = true;
  propertiesDock->hide();

  // Display dock
  QDockWidget* displayPropertiesDock = new QDockWidget( tr( "TTL_DISPLAY" ), desk );
  displayPropertiesDock->setObjectName("displayPropertiesDock");
  displayPropertiesDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, displayPropertiesDock );

  pqPropertiesPanel* displayPropertiesPanel = new pqPropertiesPanel();
  displayPropertiesPanel->setObjectName("displayPropertiesPanel");
  displayPropertiesPanel->setProperty("panelMode", QVariant(2));  // probably to have only the Display part
  displayPropertiesDock->setWidget(displayPropertiesPanel);
  myDockWidgets[displayPropertiesDock] = false;
  displayPropertiesDock->hide();

  // View dock
  QDockWidget* viewPropertiesDock = new QDockWidget( tr( "TTL_VIEW_PANEL" ), desk );
  viewPropertiesDock->setObjectName("viewPropertiesDock");
  viewPropertiesDock->setAllowedAreas( Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::LeftDockWidgetArea, viewPropertiesDock );

  pqPropertiesPanel* viewPropertiesPanel = new pqPropertiesPanel();
  viewPropertiesPanel->setObjectName("viewPropertiesPanel");
  viewPropertiesPanel->setProperty("panelMode", QVariant(4)); // probably to have only the View part
  viewPropertiesDock->setWidget(viewPropertiesPanel);
  myDockWidgets[viewPropertiesDock] = false;
  viewPropertiesDock->hide();

  // Taken from ParaViewMainWindow.cxx:
  int propertiesPanelMode = settings->value(
      "GeneralSettings.PropertiesPanelMode", vtkPVGeneralSettings::ALL_IN_ONE).toInt();
  switch (propertiesPanelMode)
  {
    case vtkPVGeneralSettings::SEPARATE_DISPLAY_PROPERTIES:
      viewPropertiesDock->hide();
      propertiesPanel->setPanelMode(
          pqPropertiesPanel::SOURCE_PROPERTIES|pqPropertiesPanel::VIEW_PROPERTIES);
      break;

    case vtkPVGeneralSettings::SEPARATE_VIEW_PROPERTIES:
      displayPropertiesDock->hide();
      propertiesPanel->setPanelMode(
          pqPropertiesPanel::SOURCE_PROPERTIES|pqPropertiesPanel::DISPLAY_PROPERTIES);
      break;

    case vtkPVGeneralSettings::ALL_SEPARATE:
      propertiesPanel->setPanelMode(pqPropertiesPanel::SOURCE_PROPERTIES);
      break;

    case vtkPVGeneralSettings::ALL_IN_ONE:
    default:
      propertiesPanel->setPanelMode(
                pqPropertiesPanel::SOURCE_PROPERTIES|pqPropertiesPanel::VIEW_PROPERTIES|pqPropertiesPanel::DISPLAY_PROPERTIES);
      viewPropertiesDock->hide();
      displayPropertiesDock->hide();
      break;
  }

  // Information dock
  QDockWidget* informationDock = new QDockWidget(tr( "TTL_INFORMATION" ), desk);
  informationDock->setObjectName("informationDock");

  QWidget* informationWidgetFrame = new QWidget(informationDock);
  informationWidgetFrame->setObjectName("informationWidgetFrame");

  QVBoxLayout* verticalLayout_2 = new QVBoxLayout(informationWidgetFrame);
  verticalLayout_2->setSpacing(0);
  verticalLayout_2->setContentsMargins(0, 0, 0, 0);

  QScrollArea* informationScrollArea = new QScrollArea(informationWidgetFrame);
  informationScrollArea->setObjectName("informationScrollArea") ;
  informationScrollArea->setWidgetResizable(true);

  pqProxyInformationWidget* informationWidget = new pqProxyInformationWidget();
  informationWidget->setObjectName("informationWidget");
  informationWidget->setGeometry(QRect(0, 0, 77, 214));
  informationScrollArea->setWidget(informationWidget);

  verticalLayout_2->addWidget(informationScrollArea);
  informationDock->setWidget(informationWidgetFrame);

  myDockWidgets[informationDock] = true;

  desk->tabifyDockWidget(propertiesDock, viewPropertiesDock);
  desk->tabifyDockWidget(propertiesDock, displayPropertiesDock);
  desk->tabifyDockWidget(propertiesDock, informationDock);
  propertiesDock->raise();

  // Statistic View
  QDockWidget* statisticsViewDock  = new QDockWidget( tr( "TTL_STATISTICS_VIEW" ), desk );
  statisticsViewDock->setObjectName("statisticsViewDock");
  statisticsViewDock->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|
                                      Qt::NoDockWidgetArea|Qt::RightDockWidgetArea );
  desk->addDockWidget( Qt::BottomDockWidgetArea, statisticsViewDock );
  pqDataInformationWidget* aStatWidget = new pqDataInformationWidget(statisticsViewDock);
  statisticsViewDock->setWidget(aStatWidget);
  myDockWidgets[statisticsViewDock] = false; // hidden by default

  //Animation view
  QDockWidget* animationViewDock     = new QDockWidget( tr( "TTL_ANIMATION_VIEW" ), desk );
  animationViewDock->setObjectName("animationViewDock");
  desk->addDockWidget( Qt::BottomDockWidgetArea, animationViewDock );
  pqPVAnimationWidget* animation_panel = new ResizeHelper(animationViewDock); // [ABN] was resizeHelper
  animationViewDock->setWidget(animation_panel);
  myDockWidgets[animationViewDock] = false; // hidden by default

  desk->tabifyDockWidget(animationViewDock,  statisticsViewDock);

  // Selection inspector
  QDockWidget* selectionDisplayDock = new QDockWidget( tr( "TTL_SELECTION_INSPECTOR" ), desk );
  selectionDisplayDock->setObjectName("selectionInspectorDock");
  selectionDisplayDock->setAllowedAreas( Qt::AllDockWidgetAreas );
  desk->addDockWidget( Qt::LeftDockWidgetArea, selectionDisplayDock );
  pqFindDataSelectionDisplayFrame* aSelInspector = new pqFindDataSelectionDisplayFrame(selectionDisplayDock);
  selectionDisplayDock->setWidget(aSelInspector);
  myDockWidgets[selectionDisplayDock] = false; // hidden by default

  // Multi-block inspector
  QDockWidget* multiBlockInspectorPanelDock  = new QDockWidget( tr( "TTL_MUTLI_BLOCK_INSPECTOR" ), desk );
  multiBlockInspectorPanelDock->setObjectName("multiBlockInspectorPanelDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, multiBlockInspectorPanelDock );
  pqMultiBlockInspectorPanel* mbi_panel = new pqMultiBlockInspectorPanel( multiBlockInspectorPanelDock );
  multiBlockInspectorPanelDock->setWidget(mbi_panel);
  myDockWidgets[multiBlockInspectorPanelDock] = false; // hidden by default

  // Comparative View
  QDockWidget* comparativePanelDock  = new QDockWidget( tr( "TTL_COMPARATIVE_VIEW_INSPECTOR" ), desk );
  comparativePanelDock->setObjectName("comparativePanelDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, comparativePanelDock );
  pqComparativeVisPanel* cv_panel = new pqComparativeVisPanel( comparativePanelDock );
  comparativePanelDock->setWidget(cv_panel);
  myDockWidgets[comparativePanelDock] = false; // hidden by default

  // Collaboration view
  QDockWidget* collaborationPanelDock = new QDockWidget(tr( "TTL_COLLABORATIVE_DOCK" ), desk);
  collaborationPanelDock->setObjectName("collaborationPanelDock");
  pqCollaborationPanel* collaborationPanel = new pqCollaborationPanel();
  collaborationPanel->setObjectName("collaborationPanel");
  collaborationPanelDock->setWidget(collaborationPanel);
  desk->addDockWidget(Qt::RightDockWidgetArea, collaborationPanelDock);
  myDockWidgets[collaborationPanelDock] = false; // hidden by default

  // Color map editor
  QDockWidget* colorMapEditorDock  = new QDockWidget( tr( "TTL_COLOR_MAP_EDITOR" ), desk );
  colorMapEditorDock->setObjectName("colorMapEditorDock");
  desk->addDockWidget( Qt::LeftDockWidgetArea, colorMapEditorDock );
  pqColorMapEditor* cmed_panel = new pqColorMapEditor( colorMapEditorDock );
  colorMapEditorDock->setWidget(cmed_panel);
  myDockWidgets[colorMapEditorDock] = false; // hidden by default

  // Provide access to the color-editor panel for the application.
  pqApplicationCore::instance()->registerManager(
    "COLOR_EDITOR_PANEL", colorMapEditorDock);
  
  // Memory inspector dock
  QDockWidget* memoryInspectorDock = new QDockWidget(tr( "TTL_MEMORY_INSPECTOR" ), desk);
  memoryInspectorDock->setObjectName("memoryInspectorDock");
#ifndef WIN32
  pqMemoryInspectorPanel* dockWidgetContents = new pqMemoryInspectorPanel();
  dockWidgetContents->setObjectName("dockWidgetContents");
  memoryInspectorDock->setWidget(dockWidgetContents);
#endif
  desk->addDockWidget(Qt::RightDockWidgetArea, memoryInspectorDock);
  myDockWidgets[memoryInspectorDock] = false; // hidden by default

  // Setup the statusbar ...
  pqProgressManager* progress_manager =
    pqApplicationCore::instance()->getProgressManager();

  // Progress bar/button management
  pqProgressWidget* aProgress = new pqProgressWidget(desk->statusBar());
  progress_manager->addNonBlockableObject(aProgress);
  progress_manager->addNonBlockableObject(aProgress->getAbortButton());
  
  QObject::connect( progress_manager, SIGNAL(enableProgress(bool)),
                    aProgress,        SLOT(enableProgress(bool)));

  QObject::connect( progress_manager, SIGNAL(progress(const QString&, int)),
                    aProgress,        SLOT(setProgress(const QString&, int)));

  QObject::connect( progress_manager, SIGNAL(enableAbort(bool)),
                    aProgress,        SLOT(enableAbort(bool)));

  QObject::connect( aProgress,        SIGNAL(abortPressed()),
                    progress_manager, SLOT(triggerAbort()));

  desk->statusBar()->addPermanentWidget(aProgress);
  aProgress->setEnabled(true);

  // Set up the dock window corners to give the vertical docks more room.
//  desk->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
//  desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  // Setup the default dock configuration ...
  statisticsViewDock->hide();
  comparativePanelDock->hide();
  animationViewDock->hide();
  multiBlockInspectorPanelDock->hide();
  selectionDisplayDock->hide();
  collaborationPanelDock->hide();
  memoryInspectorDock->hide();
  colorMapEditorDock->hide();
}

/*!
  \brief Save states of dockable ParaView widgets.
*/
void PVGUI_Module::saveDockWidgetsState()
{
  SUIT_Desktop* desk = application()->desktop();

  // VSR: 19/06/2011: do not use Paraview's methods, since it conflicts with SALOME GUI architecture
  // ... the following code is commented...
  // Save the state of the window ...
  // pqApplicationCore::instance()->settings()->saveState(*desk, "MainWindow");
  //
  //for (int i = 0; i < myDockWidgets.size(); ++i)
  //  myDockWidgets.at(i)->setParent(0);
  // ... and replaced - manually hide dock windows

  // store dock widgets visibility state and hide'em all
  QMapIterator<QWidget*, bool> it1( myDockWidgets );
  while( it1.hasNext() ) {
    it1.next();
    QDockWidget* dw = qobject_cast<QDockWidget*>( it1.key() );
    myDockWidgets[dw] = dw->isVisible();
    dw->setVisible( false );
    dw->toggleViewAction()->setVisible( false );
  }
  // store toolbar breaks state and remove all tollbar breaks 
  QMapIterator<QWidget*, bool> it2( myToolbarBreaks );
  while( it2.hasNext() ) {
    it2.next();
    QToolBar* tb = qobject_cast<QToolBar*>( it2.key() );
    myToolbarBreaks[tb] = desk->toolBarBreak( tb );
    if ( myToolbarBreaks[tb] )
      desk->removeToolBarBreak( tb );
  }
  // store toolbars visibility state and hide'em all
  QMapIterator<QWidget*, bool> it3( myToolbars );
  while( it3.hasNext() ) {
    it3.next();
    QToolBar* tb = qobject_cast<QToolBar*>( it3.key() );
    myToolbars[tb] = tb->isVisible();
    tb->setVisible( false );
    tb->toggleViewAction()->setVisible( false );
  }
}

/*!
  \brief Restore states of dockable ParaView widgets.
*/
void PVGUI_Module::restoreDockWidgetsState()
{
  SUIT_Desktop* desk = application()->desktop();

  // VSR: 19/06/2011: do not use Paraview's methods, since it conflicts with SALOME GUI architecture
  // ... the following code is commented...
  //for (int i = 0; i < myDockWidgets.size(); ++i)
  //  myDockWidgets.at(i)->setParent(desk);
  //
  // Restore the state of the window ...
  //pqApplicationCore::instance()->settings()->restoreState("MainWindow", *desk);
  // ... and replaced - manually hide dock windows

  // restore dock widgets visibility state
  QMapIterator<QWidget*, bool> it1( myDockWidgets );
  while( it1.hasNext() ) {
    it1.next();
    QDockWidget* dw = qobject_cast<QDockWidget*>( it1.key() );
    dw->setVisible( it1.value() );
    dw->toggleViewAction()->setVisible( true );
  }

    // restore toolbar breaks state
    QMapIterator<QWidget*, bool> it2( myToolbarBreaks );
    while( it2.hasNext() ) {
        it2.next();
        QToolBar* tb = qobject_cast<QToolBar*>( it2.key() );
        if ( myToolbarBreaks[tb] )
          desk->insertToolBarBreak( tb );
    }

  // restore toolbar visibility state
  QMapIterator<QWidget*, bool> it3( myToolbars );
  while( it3.hasNext() ) {
    it3.next();
    QToolBar* tb = qobject_cast<QToolBar*>( it3.key() );
    tb->setVisible( it3.value() );
    tb->toggleViewAction()->setVisible( true );
  }
}



/*!
  \brief Store visibility of the common dockable windows (OB, PyConsole, ... etc.)
*/
void PVGUI_Module::storeCommonWindowsState() {  
  //rnv: Make behaviour of the dockable windows and toolbars coherent with others
  //     modules: if 'Save position of the windows' or 'Save position of the toolbars' 
  //     in the General SALOME preferences are cheked, then properties of the windows and/or toolbars
  //     are stored/restored using standard Qt saveState(...) and restoreState(...) methods.
  //     Otherwise to the windows and toolbars applied default settins stored int the SalomeApp.xml
  //     configuration file.
  //
  //     But in contrast to others modules ParaVis module default settings hide some dockable
  //     windows, so to restore it at the moment of the ParaVis de-activation we call 
  //     restoreCommonWindowsState() method, and at the moment of the ParaVis activation we call 
  //     this method.

  //LightApp_Application* anApp = getApp();
  SalomeApp_Application* anApp = getApp();
  if(!anApp)
    return;

//  int begin = SalomeApp_Application::WT_ObjectBrowser;
//  int end = SalomeApp_Application::WT_NoteBook;
  int begin = LightApp_Application::WT_ObjectBrowser;
  int end = LightApp_Application::WT_User;
  for( int i = begin; i <= end; i++ ) {
    QWidget* wg = anApp->getWindow(i);
    if(wg) {
      QDockWidget* dock = 0;
      QWidget* w = wg->parentWidget();
      while ( w && !dock ) {
          dock = ::qobject_cast<QDockWidget*>( w );
          w = w->parentWidget();
      }
      if(dock){
          if(!myCommonMap.contains(i)){
              myCommonMap.insert(i,dock->isVisible());
          } else {
              myCommonMap[i] = dock->isVisible();
          }
      }
    }
  }
}

/*!
  \brief Restore visibility of the common dockable windows (OB, PyConsole, ... etc.)
*/
void PVGUI_Module::restoreCommonWindowsState() {
  SalomeApp_Application* anApp = getApp();
//  LightApp_Application* anApp = getApp();
  if(!anApp)
    return;
  DockWindowMap::const_iterator it = myCommonMap.begin();
  for( ;it != myCommonMap.end(); it++ ) {
    QWidget* wg = anApp->getWindow(it.key());
    if(wg) {
      QDockWidget* dock = 0;
      QWidget* w = wg->parentWidget();
      while ( w && !dock ) {
          dock = ::qobject_cast<QDockWidget*>( w );
          w = w->parentWidget();
      }
      if(dock) {
          dock->setVisible(it.value());
      }
    }
  }
}
