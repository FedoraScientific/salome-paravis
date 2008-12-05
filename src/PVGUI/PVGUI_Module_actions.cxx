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

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <LightApp_Application.h>

#include <QAction>
#include <QMenu>
#include <QToolBar>

#include <pqApplicationCore.h>
#include <pqColorScaleToolbar.h>
#include <pqMainWindowCore.h>
#include <pqObjectInspectorDriver.h>
#include <pqPipelineBrowser.h>
#include <pqProgressManager.h>
#include <pqRubberBandHelper.h>
#include <pqScalarBarVisibilityAdaptor.h>
#include <pqUndoStack.h>
#include <pqVCRController.h>

/*!
  \brief Create actions for ParaView GUI operations
  duplicating menus and toolbars in MainWindow class of
  the standard ParaView GUI client application.
*/
void PVGUI_Module::pvCreateActions()
{
  SUIT_Desktop* desk = application()->desktop();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap aPixmap;
  QAction* anAction;
  QtxAction* aQtxAction;
  
  // --- Menu "File"

  // Open File
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_OPEN_FILE"), false );
  createAction( OpenFileId, tr("TOP_OPEN_FILE"), QIcon(aPixmap),
                tr("MEN_OPEN"), tr("STB_OPEN_FILE"), 
		0, desk, false, &Implementation->Core, SLOT( onFileOpen() ) );

  // Load State
  anAction = createAction( LoadStateId, tr("TOP_LOAD_STATE"), QIcon(),
			   tr("MEN_LOAD_STATE"), tr("STB_LOAD_STATE"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileLoadServerState() ) );
  connect( &Implementation->Core, SIGNAL( enableFileLoadServerState(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Save State
  anAction = createAction( SaveStateId, tr("TOP_SAVE_STATE"), QIcon(),
			   tr("MEN_SAVE_STATE"), tr("STB_SAVE_STATE"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileSaveServerState() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveServerState(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Save Data
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SAVE_DATA"), false );
  anAction = createAction( SaveDataId, tr("TOP_SAVE_DATA"), QIcon(aPixmap),
			   tr("MEN_SAVE_DATA"), tr("STB_SAVE_DATA"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileSaveData() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveData(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Save Screenshot
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SAVE_SCREENSHOT"), false );
  anAction = createAction( SaveScreenshotId, tr("TOP_SAVE_SCREENSHOT"), QIcon(aPixmap),
			   tr("MEN_SAVE_SCREENSHOT"), tr("STB_SAVE_SCREENSHOT"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileSaveScreenshot() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Export
  anAction = createAction( ExportId, tr("TOP_EXPORT"), QIcon(),
			   tr("MEN_EXPORT"), tr("STB_EXPORT"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileExport() ) );
  connect( &Implementation->Core, SIGNAL( enableExport(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Save Animation
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SAVE_ANIMATION"), false );
  anAction = createAction( SaveAnimationId, tr("TOP_SAVE_ANIMATION"), QIcon(aPixmap),
			   tr("MEN_SAVE_ANIMATION"), tr("STB_SAVE_ANIMATION"), 
			   0, desk, false, &Implementation->Core, SLOT( onFileSaveAnimation() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveAnimation(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Save Geometry
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SAVE_GEOMETRY"), false );
  anAction = createAction( SaveGeometryId, tr("TOP_SAVE_GEOMETRY"), QIcon(aPixmap),
			   tr("MEN_SAVE_GEOMETRY"), tr("STB_SAVE_GEOMETRY"), 
			   0, desk, false, &Implementation->Core, SLOT( onSaveGeometry() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveGeometry(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Connect
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_CONNECT"), false );
  anAction = createAction( ConnectId, tr("TOP_CONNECT"), QIcon(aPixmap),
			   tr("MEN_CONNECT"), tr("STB_CONNECT"), 
			   0, desk, false, &Implementation->Core, SLOT( onServerConnect() ) );
  connect( &Implementation->Core, SIGNAL( enableServerConnect(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Disconnect
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_DISCONNECT"), false );
  anAction = createAction( DisconnectId, tr("TOP_DISCONNECT"), QIcon(aPixmap),
			   tr("MEN_DISCONNECT"), tr("STB_DISCONNECT"), 
			   0, desk, false, &Implementation->Core, SLOT( onServerDisconnect() ) );
  connect( &Implementation->Core, SIGNAL( enableServerDisconnect(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // --- Menu "Edit"

  pqUndoStack* undoStack = Implementation->Core.getApplicationUndoStack();

  // Undo
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_UNDO"), false );
  anAction = createAction( UndoId, tr("TOP_UNDO"), QIcon(aPixmap),
			   tr("MEN_UNDO"), tr("STB_UNDO"), Qt::CTRL+Qt::Key_Z,
			   desk, false, undoStack, SLOT( undo() ) );
  connect( undoStack, SIGNAL( canUndoChanged(bool) ),
	   anAction,  SLOT( setEnabled(bool) ) );
  connect( undoStack, SIGNAL( undoLabelChanged(const QString&) ),
	   this,      SLOT( onUndoLabel(const QString&) ) );
  anAction->setEnabled( undoStack->canUndo() );
  onUndoLabel(undoStack->undoLabel());

  // Redo
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_REDO"), false );
  anAction = createAction( RedoId, tr("TOP_REDO"), QIcon(aPixmap),
			   tr("MEN_REDO"), tr("STB_REDO"), Qt::CTRL+Qt::Key_Y,
			   desk, false, undoStack, SLOT( redo() ) );
  connect( undoStack, SIGNAL( canRedoChanged(bool) ),	
	   anAction,  SLOT( setEnabled(bool) ) );
  connect( undoStack, SIGNAL( redoLabelChanged(const QString&) ),
	   this,      SLOT( onRedoLabel(const QString&) ) );
  anAction->setEnabled( undoStack->canRedo() );
  onRedoLabel(undoStack->redoLabel());

  // Camera Undo
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_CAMERA_UNDO"), false );
  anAction = createAction( CameraUndoId, tr("TOP_CAMERA_UNDO"), QIcon(aPixmap),
			   tr("MEN_CAMERA_UNDO"), tr("STB_CAMERA_UNDO"), Qt::CTRL+Qt::Key_C+Qt::Key_B,
			   desk, false, &Implementation->Core, SLOT( onEditCameraUndo() ) );
  connect( &Implementation->Core, SIGNAL( enableCameraUndo(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );
  connect( &Implementation->Core, SIGNAL( cameraUndoLabel(const QString&) ),
	   this,                  SLOT( onCameraUndoLabel(const QString&) ) );
  
  // Camera Redo
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_CAMERA_REDO"), false );
  anAction = createAction( CameraRedoId, tr("TOP_CAMERA_REDO"), QIcon(aPixmap),
			   tr("MEN_CAMERA_REDO"), tr("STB_CAMERA_REDO"), Qt::CTRL+Qt::Key_C+Qt::Key_F,
			   desk, false, &Implementation->Core, SLOT( onEditCameraRedo() ) );
  connect( &Implementation->Core, SIGNAL( enableCameraRedo(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );
  connect( &Implementation->Core, SIGNAL( cameraRedoLabel(const QString&) ),
	   this,                  SLOT( onCameraRedoLabel(const QString&) ) );

  pqPipelineBrowser* browser = Implementation->Core.pipelineBrowser();

  // Change Input
  createAction( ChangeInputId, tr("TOP_CHANGE_INPUT"), QIcon(),
		tr("MEN_CHANGE_INPUT"), tr("STB_CHANGE_INPUT"), 
		0, desk, false, browser, SLOT( changeInput() ) );

  // Delete
  createAction( DeleteId, tr("TOP_DELETE"), QIcon(),
		tr("MEN_DELETE"), tr("STB_DELETE"),
		0, desk, false, browser, SLOT( deleteSelected() ) );

  // Delet All
  createAction( DeleteAllId, tr("TOP_DELETE_ALL"), QIcon(),
		tr("MEN_DELETE_ALL"), tr("STB_DELETE_ALL"),
		0, desk, false, this, SLOT( onDeleteAll() ) );

  // Interact
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_INTERACT"), false );
  anAction = createAction( InteractId, tr("TOP_INTERACT"), QIcon(aPixmap),
			   tr("MEN_INTERACT"), tr("STB_INTERACT"), 0, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( endSelection() ) );
  anAction->setChecked( true ); ///!!!
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( interactionModeChanged(bool) ),
	   anAction,                                         SLOT( setChecked(bool) ) );

  // Select Cells On
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SELECT_CELLS_ON"), false );
  anAction = createAction( SelectCellsOnId, tr("TOP_SELECT_CELLS_ON"), QIcon(aPixmap), 
			   tr("MEN_SELECT_CELLS_ON"), tr("STB_SELECT_CELLS_ON"), Qt::Key_S, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( beginSurfaceSelection() ) );
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( enableSurfaceSelection(bool) ),
	   anAction,                                         SLOT( setEnabled(bool) ) );

  // Select Points On
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SELECT_POINTS_ON"), false );
  anAction = createAction( SelectPointsOnId, tr("TOP_SELECT_POINTS_ON"), QIcon(aPixmap), 
			   tr("MEN_SELECT_POINTS_ON"), tr("STB_SELECT_POINTS_ON"), 0, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( beginSurfacePointsSelection() ) );
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( enableSurfacePointsSelection(bool) ),
	   anAction,                                         SLOT( setEnabled(bool) ) );
  
  // Select Cells Through
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SELECT_CELLS_THROUGH"), false );
  anAction = createAction( SelectCellsThroughId, tr("TOP_SELECT_CELLS_THROUGH"), QIcon(aPixmap), 
			   tr("MEN_SELECT_CELLS_THROUGH"), tr("STB_SELECT_CELLS_THROUGH"), 0, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( beginFrustumSelection() ) );
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( enableFrustumSelection(bool) ),
	   anAction,                                         SLOT( setEnabled(bool) ) );

  // Select Points Through
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SELECT_POINTS_THROUGH"), false );
  anAction = createAction( SelectPointsThroughId, tr("TOP_SELECT_POINTS_THROUGH"), QIcon(aPixmap), 
			   tr("MEN_SELECT_POINTS_THROUGH"), tr("STB_SELECT_POINTS_THROUGH"), 0, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( beginFrustumPointsSelection() ) );
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( enableFrustumPointSelection(bool) ),
	   anAction,                                         SLOT( setEnabled(bool) ) );

  // Select Block
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SELECT_BLOCK"), false );
  anAction = createAction( SelectBlockId, tr("TOP_SELECT_BLOCK"), QIcon(aPixmap), 
			   tr("MEN_SELECT_BLOCK"), tr("STB_SELECT_BLOCK"), Qt::Key_B, desk, true,
			   Implementation->Core.renderViewSelectionHelper(), SLOT( beginBlockSelection() ) );
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( enableBlockSelection(bool) ),
	   anAction,                                         SLOT( setEnabled(bool) ) );

  // Create action group for exclusively selected modes
  QActionGroup* aModeGroup = new QActionGroup( desk );
  aModeGroup->addAction( action(InteractId) );
  aModeGroup->addAction( action(SelectCellsOnId) );
  aModeGroup->addAction( action(SelectPointsOnId) );
  aModeGroup->addAction( action(SelectCellsThroughId) );
  aModeGroup->addAction( action(SelectPointsThroughId) );
  aModeGroup->addAction( action(SelectBlockId) );
  
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( selectionModeChanged(int) ),
	   this,                                             SLOT( onSelectionModeChanged(int) ) );
  // When a selection is marked, we revert to interaction mode.
  connect( Implementation->Core.renderViewSelectionHelper(), SIGNAL( selectionFinished(int, int, int, int) ),
	   Implementation->Core.renderViewSelectionHelper(), SLOT( endSelection() ) );

  // Setting
  createAction( SettingsId, tr("TOP_SETTINGS"), QIcon(),
		tr("MEN_SETTINGS"), tr("STB_SETTINGS"),
		0, desk, false, &Implementation->Core, SLOT( onEditSettings() ) );
  
  // View Settings
  createAction( ViewSettingsId, tr("TOP_VIEW_SETTINGS"), QIcon(),
		tr("MEN_VIEW_SETTINGS"), tr("STB_VIEW_SETTINGS"),
		0, desk, false, &Implementation->Core, SLOT( onEditViewSettings() ) );

  // --- Menu "View"

  // Camera submenu -> Reset
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_RESET_CAMERA"), false );
  anAction = createAction( ResetCameraId, tr("TOP_RESET_CAMERA"), QIcon(aPixmap), 
			   tr("MEN_RESET_CAMERA"), tr("STB_RESET_CAMERA"),
			   0, desk, false, &Implementation->Core, SLOT( resetCamera() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> +X
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_+X"), false );
  anAction = createAction( PositiveXId, tr("TOP_+X"), QIcon(aPixmap), 
			   tr("MEN_+X"), tr("STB_+X"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionPosX() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> -X
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_-X"), false );
  anAction = createAction( NegativeXId, tr("TOP_-X"), QIcon(aPixmap), 
			   tr("MEN_-X"), tr("STB_-X"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionNegX() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> +Y
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_+Y"), false );
  anAction = createAction( PositiveYId, tr("TOP_+Y"), QIcon(aPixmap), 
			   tr("MEN_+Y"), tr("STB_+Y"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionPosY() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> -Y
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_-Y"), false );
  anAction = createAction( NegativeYId, tr("TOP_-Y"), QIcon(aPixmap), 
			   tr("MEN_-Y"), tr("STB_-Y"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionNegY() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> +Z
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_+Z"), false );
  anAction = createAction( PositiveZId, tr("TOP_+Z"), QIcon(aPixmap), 
			   tr("MEN_+Z"), tr("STB_+Z"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionPosZ() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Camera submenu -> -Z
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_-Z"), false );
  anAction = createAction( NegativeZId, tr("TOP_-Z"), QIcon(aPixmap), 
			   tr("MEN_-Z"), tr("STB_-Z"),
			   0, desk, false, &Implementation->Core, SLOT( resetViewDirectionNegZ() ) );
  connect( &Implementation->Core, SIGNAL( enableFileSaveScreenshot(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Show Center
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SHOW_CENTER"), false );
  aQtxAction = new QtxAction( tr("TOP_SHOW_CENTER"), QIcon(aPixmap), 
			      tr("MEN_SHOW_CENTER"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_SHOW_CENTER" ) );
  aQtxAction->setCheckable( true );
  registerAction( ShowCenterId, aQtxAction );
  connect( aQtxAction,            SIGNAL( toggled(bool) ),
	   &Implementation->Core, SLOT( setCenterAxesVisibility(bool) ) );
  connect( &Implementation->Core, SIGNAL( enableShowCenterAxis(bool) ),
	   this,                  SLOT( onShowCenterAxisChanged(bool) ), Qt::QueuedConnection );

  // Reset Center
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_RESET_CENTER"), false );
  anAction = createAction( ResetCenterId, tr("TOP_RESET_CENTER"), QIcon(aPixmap), 
			   tr("MEN_RESET_CENTER"), tr("STB_RESET_CENTER"), 0, desk, false,
			   &Implementation->Core, SLOT( resetCenterOfRotationToCenterOfCurrentData() ) );
  connect( &Implementation->Core, SIGNAL( enableResetCenter(bool) ),
	   anAction,              SLOT( setEnabled(bool) ) );

  // Pick Center
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_PICK_CENTER"), false );
  aQtxAction = new QtxAction( tr("TOP_PICK_CENTER"), QIcon(aPixmap), 
			      tr("MEN_PICK_CENTER"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_PICK_CENTER" ) );
  aQtxAction->setCheckable( true );
  registerAction( PickCenterId, aQtxAction );
  connect( aQtxAction,            SIGNAL( toggled(bool) ),
	   &Implementation->Core, SLOT( pickCenterOfRotation(bool) ) );
  connect( &Implementation->Core, SIGNAL( enablePickCenter(bool) ),
	   aQtxAction,            SLOT( setEnabled(bool) ) );
  connect( &Implementation->Core, SIGNAL( pickingCenter(bool) ),
	   aQtxAction,            SLOT( setChecked(bool) ) );

  // Show Color Legend
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_SHOW_COLOR_LEGEND"), false );
  aQtxAction = new QtxAction( tr("TOP_SHOW_COLOR_LEGEND"), QIcon(aPixmap), 
			      tr("MEN_SHOW_COLOR_LEGEND"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_SHOW_COLOR_LEGEND" ) );
  aQtxAction->setCheckable( true );
  aQtxAction->setEnabled(false);
  registerAction( ShowColorLegendId, aQtxAction );
  pqScalarBarVisibilityAdaptor* sbva = new pqScalarBarVisibilityAdaptor( aQtxAction );
  connect( Implementation->Core.getObjectInspectorDriver(),
	   SIGNAL( representationChanged(pqDataRepresentation*, pqView*) ),
	   sbva,
	   SLOT( setActiveRepresentation(pqDataRepresentation*) ) );

  // --- Menu "Sources" and "Filters"
  // As ParaView is responsible for updating "Sources" and "Filters" menus,
  // actions for these menus are created inside the pqMainWindowCore.
  // So, the creation of corresponding QMenu object is in the PVGUI_Module::pvCreateMenus().

  // --- Menu "Animation"

  pqVCRController* vcrcontroller = &Implementation->Core.VCRController();

  // First Frame
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_FIRST_FRAME"), false );
  anAction = createAction( FirstFrameId, tr("TOP_FIRST_FRAME"), QIcon(aPixmap), 
			   tr("MEN_FIRST_FRAME"), tr("STB_FIRST_FRAME"),
			   0, desk, false, vcrcontroller, SLOT( onFirstFrame() ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), anAction, SLOT( setEnabled(bool) ) );

  // Previous Frame
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_PREVIOUS_FRAME"), false );
  anAction = createAction( PreviousFrameId, tr("TOP_PREVIOUS_FRAME"), QIcon(aPixmap), 
			   tr("MEN_PREVIOUS_FRAME"), tr("STB_PREVIOUS_FRAME"),
			   0, desk, false, vcrcontroller, SLOT( onPreviousFrame() ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), anAction, SLOT( setEnabled(bool) ) );

  // Play
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_PLAY"), false );
  anAction = createAction( PlayId, tr("TOP_PLAY"), QIcon(aPixmap), 
			   tr("MEN_PLAY"), tr("STB_PLAY"),
			   0, desk, false, vcrcontroller, SLOT( onPlay() ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), anAction, SLOT( setEnabled(bool) ) );

  // Next Frame
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_NEXT_FRAME"), false );
  anAction = createAction( NextFrameId, tr("TOP_NEXT_FRAME"), QIcon(aPixmap), 
			   tr("MEN_NEXT_FRAME"), tr("STB_NEXT_FRAME"),
			   0, desk, false, vcrcontroller, SLOT( onNextFrame() ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), anAction, SLOT( setEnabled(bool) ) );

  // Last Frame
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_LAST_FRAME"), false );
  anAction = createAction( LastFrameId, tr("TOP_LAST_FRAME"), QIcon(aPixmap), 
			   tr("MEN_LAST_FRAME"), tr("STB_LAST_FRAME"),
			   0, desk, false, vcrcontroller, SLOT( onLastFrame() ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), anAction, SLOT( setEnabled(bool) ) );

  // Loop
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_LOOP"), false );
  aQtxAction = new QtxAction( tr("TOP_LOOP"), QIcon(aPixmap), 
			      tr("MEN_LOOP"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_LOOP" ) );
  aQtxAction->setCheckable(true);
  registerAction( LoopId, aQtxAction );
  connect( aQtxAction,    SIGNAL( toggled(bool) ), vcrcontroller, SLOT( onLoop(bool) ) );
  connect( vcrcontroller, SIGNAL( enabled(bool) ), aQtxAction,    SLOT( setEnabled(bool) ) );
  connect( vcrcontroller, SIGNAL( loop(bool) ),    aQtxAction,    SLOT( setChecked(bool) ) );

  connect( vcrcontroller, SIGNAL( playing(bool) ), this, SLOT( onPlaying(bool) ) );
  connect( vcrcontroller, SIGNAL( timeRanges(double, double) ),
	   this,          SLOT( setTimeRanges(double, double) ) );

  // --- Menu "Tools"

  // Create Custom Filter
  createAction( CreateCustomFilterId, tr("TOP_CREATE_CUSTOM_FILTER"), QIcon(), 
		tr("MEN_CREATE_CUSTOM_FILTER"), tr("STB_CREATE_CUSTOM_FILTER"),
		0, desk, false, &Implementation->Core, SLOT( onToolsCreateCustomFilter() ) );
  
  // Manage Custom Filters
  createAction( ManageCustomFiltersId, tr("TOP_MANAGE_CUSTOM_FILTERS"), QIcon(), 
		tr("MEN_MANAGE_CUSTOM_FILTERS"), tr("STB_MANAGE_CUSTOM_FILTERS"),
		0, desk, false, &Implementation->Core, SLOT( onToolsManageCustomFilters() ) );

  // Create Lookmark
  createAction( CreateLookmarkId, tr("TOP_CREATE_LOOKMARK"), QIcon(), 
		tr("MEN_CREATE_LOOKMARK"), tr("STB_CREATE_LOOKMARK"),
		0, desk, false, &Implementation->Core, SLOT( onToolsCreateLookmark() ) );

  // Manage Links
  createAction( ManageLinksId, tr("TOP_MANAGE_LINKS"), QIcon(), 
		tr("MEN_MANAGE_LINKS"), tr("STB_MANAGE_LINKS"),
		0, desk, false, &Implementation->Core, SLOT( onToolsManageLinks() ) );

  // Add Camera Link
  createAction( AddCameraLinkId, tr("TOP_ADD_CAMERA_LINK"), QIcon(), 
		tr("MEN_ADD_CAMERA_LINK"), tr("STB_ADD_CAMERA_LINK"),
		0, desk, false, this, SLOT( onAddCameraLink() ) );

  // Manage Plugins/Extensions
  createAction( ManagePluginsExtensionsId, tr( "TOP_MANAGE_PLUGINS" ), QIcon(),
		tr( "MEN_MANAGE_PLUGINS" ), tr( "STB_MANAGE_PLUGINS" ),
		0, desk, false, &Implementation->Core, SLOT( onManagePlugins() ) );

  // Dump Widget Names
  createAction( DumpWidgetNamesId, tr( "TOP_DUMP_WIDGET_NAMES" ), QIcon(),
		tr( "MEN_DUMP_WIDGET_NAMES" ), tr( "STB_DUMP_WIDGET_NAMES" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsDumpWidgetNames() ) );

  // Record Test
  createAction( RecordTestId, tr( "TOP_RECORD_TEST" ), QIcon(),
		tr( "MEN_RECORD_TEST" ), tr( "STB_RECORD_TEST" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsRecordTest() ) );

  // Record Test Screenshot
  createAction( RecordTestScreenshotId, tr( "TOP_RECORD_TEST_SCREENSHOT" ), QIcon(),
		tr( "MEN_RECORD_TEST_SCREENSHOT" ), tr( "STB_RECORD_TEST_SCREENSHOT" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsRecordTestScreenshot() ) );
  
  // Play Test
  createAction( PlayTestId, tr( "TOP_PLAY_TEST" ), QIcon(),
		tr( "MEN_PLAY_TEST" ), tr( "STB_PLAY_TEST" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsPlayTest() ) );

  // Max Window Size
  aQtxAction = new QtxAction( tr("TOP_MAX_WINDOW_SIZE"), QIcon(), 
			      tr("MEN_MAX_WINDOW_SIZE"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_MAX_WINDOW_SIZE" ) );
  aQtxAction->setCheckable(true);
  registerAction( MaxWindowSizeId, aQtxAction );
  connect( aQtxAction, SIGNAL( toggled(bool) ), &Implementation->Core, SLOT( enableTestingRenderWindowSize(bool) ) );

  // Timer Log
  createAction( TimerLogId, tr( "TOP_TIMER_LOG" ), QIcon(),
		tr( "MEN_TIMER_LOG" ), tr( "STB_TIMER_LOG" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsTimerLog() ) );

  // Output Window
  createAction( OutputWindowId, tr( "TOP_OUTPUT_WINDOW" ), QIcon(),
		tr( "MEN_OUTPUT_WINDOW" ), tr( "STB_OUTPUT_WINDOW" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsOutputWindow() ) );
  
  // Python Shell
  createAction( PythonShellId, tr( "TOP_PYTHON_SHELL" ), QIcon(),
		tr( "MEN_PYTHON_SHELL" ), tr( "STB_PYTHON_SHELL" ),
		0, desk, false, &Implementation->Core, SLOT( onToolsPythonShell() ) );

  // --- Menu "Help"

  // About
  createAction( AboutParaViewId, tr( "TOP_ABOUT" ), QIcon(),
		tr( "MEN_ABOUT" ), tr( "STB_ABOUT" ),
		0, desk, false, this, SLOT( onHelpAbout() ) );

  // ParaView Help
  // TODO...

  // Enable Tooltips
  aQtxAction = new QtxAction( tr("TOP_ENABLE_TOOLTIPS"), QIcon(), 
			      tr("MEN_ENABLE_TOOLTIPS"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_ENABLE_TOOLTIPS" ) );
  aQtxAction->setCheckable(true);
  aQtxAction->setChecked(true);
  registerAction( EnableTooltipsId, aQtxAction );
}

/*!
  \brief Create menus for ParaView GUI operations
  duplicating menus in pqMainWindow ParaView class

  In particular, ParaView is responsible for updating "Sources" and "Filters" menus. 
  For this, specific menu managers created by pqMainWindowCore class are used, and PVGUI_Module
  is responsible for creation of corresponding QMenu objects only.
*/
void PVGUI_Module::pvCreateMenus()
{
  // --- Menu "File"
  int aPVMnu = createMenu( tr( "MEN_DESK_FILE" ), -1, -1 );

  createMenu( OpenFileId, aPVMnu, 5 );

  // Recent Files
  int aMenuId = createMenu( tr( "MEN_RECENT_FILES" ), aPVMnu, -1, 5 );
  QMenu* aMenu = menuMgr()->findMenu( aMenuId );
  Implementation->RecentFilesMenu = new pqRecentFilesMenu( *aMenu, getApp()->desktop() );
  connect( Implementation->RecentFilesMenu, SIGNAL( serverConnectFailed() ),
	   &Implementation->Core,           SLOT( makeDefaultConnectionIfNoneExists() ) );
  QList<QAction*> anActns = aMenu->actions();
  for (int i = 0; i < anActns.size(); ++i)
    createMenu( anActns.at(i), aMenuId );


  createMenu( separator(), aPVMnu, -1, 5 );

  createMenu( LoadStateId, aPVMnu, 15 );
  createMenu( SaveStateId, aPVMnu, 15 );
  createMenu( separator(), aPVMnu, -1, 15 );

  createMenu( SaveDataId, aPVMnu, 25 );
  createMenu( SaveScreenshotId, aPVMnu, 25 );
  createMenu( ExportId, aPVMnu, 25 );
  createMenu( separator(), aPVMnu, -1, 25 );

  createMenu( SaveAnimationId, aPVMnu, 35 );
  createMenu( SaveGeometryId, aPVMnu, 35 );
  createMenu( separator(), aPVMnu, -1, 35 );

  createMenu( ConnectId, aPVMnu, 45 );
  createMenu( DisconnectId, aPVMnu, 45 );
  createMenu( separator(), aPVMnu, -1, 45 );

  // --- Menu "Edit"
  aPVMnu = createMenu( tr( "MEN_DESK_EDIT" ), -1, -1 );

  createMenu( UndoId, aPVMnu );
  createMenu( RedoId, aPVMnu );
  createMenu( separator(), aPVMnu );

  createMenu( CameraUndoId, aPVMnu );
  createMenu( CameraRedoId, aPVMnu );
  createMenu( separator(), aPVMnu );

  createMenu( ChangeInputId, aPVMnu );
  createMenu( DeleteId, aPVMnu );
  createMenu( DeleteAllId, aPVMnu );
  createMenu( separator(), aPVMnu );

  createMenu( InteractId, aPVMnu );
  createMenu( SelectCellsOnId, aPVMnu );
  createMenu( SelectPointsOnId, aPVMnu );
  createMenu( SelectCellsThroughId, aPVMnu );
  createMenu( SelectPointsThroughId, aPVMnu );
  createMenu( SelectBlockId, aPVMnu );
  createMenu( separator(), aPVMnu );

  createMenu( SettingsId, aPVMnu );
  createMenu( ViewSettingsId, aPVMnu );
  createMenu( separator(), aPVMnu );

  // --- Menu "View"
  aPVMnu = createMenu( tr( "MEN_DESK_VIEW" ), -1, -1 );

  int aCameraMnu = createMenu( tr( "MEN_CAMERA" ), aPVMnu );
  createMenu( ResetCameraId, aCameraMnu );
  createMenu( PositiveXId, aCameraMnu );
  createMenu( NegativeXId, aCameraMnu );
  createMenu( PositiveYId, aCameraMnu );
  createMenu( NegativeYId, aCameraMnu );
  createMenu( PositiveZId, aCameraMnu );
  createMenu( NegativeZId, aCameraMnu );
  createMenu( separator(), aPVMnu );

  createMenu( ShowCenterId, aPVMnu );
  createMenu( ResetCenterId, aPVMnu );
  createMenu( PickCenterId, aPVMnu );
  createMenu( ShowColorLegendId, aPVMnu );
  createMenu( separator(), aPVMnu );

  // --- Menu "Sources"

  // Install ParaView managers for "Sources" menu
  QMenu* aRes = 0;
  mySourcesMenuId = createMenu( tr( "MEN_DESK_SOURCES" ), -1, -1, 60 );
  if ( (aRes = getMenu( mySourcesMenuId )) ) {
    Implementation->Core.setSourceMenu( aRes );
    connect( &Implementation->Core, SIGNAL( enableSourceCreate(bool) ),
	     aRes,                  SLOT( setEnabled(bool) ) );
  }
  
  // --- Menu "Filters"

  // Install ParaView managers for "Filters" menu
  myFiltersMenuId = createMenu( tr( "MEN_DESK_FILTERS" ), -1, -1, 70 );
  if ( (aRes = getMenu( myFiltersMenuId )) ) {
    Implementation->Core.setFilterMenu( aRes );
    connect( &Implementation->Core, SIGNAL( enableFilterCreate(bool) ),
	     aRes,                  SLOT( setEnabled(bool) ) );
  }
  
  // --- Menu "Animation"
  int anAnimationMnu = createMenu( tr( "MEN_DESK_ANIMATION" ), -1, -1, 80 );

  createMenu( FirstFrameId, anAnimationMnu );
  createMenu( PreviousFrameId, anAnimationMnu );
  createMenu( PlayId, anAnimationMnu );
  createMenu( NextFrameId, anAnimationMnu );
  createMenu( LastFrameId, anAnimationMnu );
  createMenu( LoopId, anAnimationMnu );

  // --- Menu "Tools"

  int aToolsMnu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, -1, 90 );

  createMenu( CreateCustomFilterId, aToolsMnu );
  createMenu( ManageCustomFiltersId, aToolsMnu );
  createMenu( CreateLookmarkId, aToolsMnu );
  createMenu( ManageLinksId, aToolsMnu );
  createMenu( AddCameraLinkId, aToolsMnu );
  createMenu( ManagePluginsExtensionsId, aToolsMnu );
  createMenu( separator(), aToolsMnu );

  createMenu( DumpWidgetNamesId, aToolsMnu );
  createMenu( RecordTestId, aToolsMnu );
  createMenu( RecordTestScreenshotId, aToolsMnu );
  createMenu( PlayTestId, aToolsMnu );
  createMenu( MaxWindowSizeId, aToolsMnu );
  createMenu( separator(), aToolsMnu );

  createMenu( TimerLogId, aToolsMnu );
  createMenu( OutputWindowId, aToolsMnu );
  createMenu( separator(), aToolsMnu );
  
  createMenu( PythonShellId, aToolsMnu );

  // --- Menu "Help"

  int aHelpMnu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1 );

  createMenu( AboutParaViewId, aHelpMnu );
  // ParaView Help
  // It's needed to install module docs into <module>_ROOT_DIR/share/doc/salome/gui/<module>
  // createMenu( ParaViewHelpId, aHelpMnu );
  createMenu( EnableTooltipsId, aHelpMnu );
  createMenu( separator(), aHelpMnu );

}

/*!
  \brief Create toolbars for ParaView GUI operations
  duplicating toolbars in pqMainWindow ParaView class
*/
void PVGUI_Module::pvCreateToolBars()
{
  SUIT_Desktop* desk = application()->desktop();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap aPixmap;
  QtxAction* aQtxAction;

  // --- Toolbar "Main Controls"

  int aToolId = createTool( tr("TOOL_MAIN_CONTROLS") );

  createTool( OpenFileId, aToolId );
  createTool( SaveDataId, aToolId );
  createTool( separator(), aToolId );
  createTool( ConnectId, aToolId );
  createTool( DisconnectId, aToolId );
  //createTool( separator(), aToolId );
  //createTool( ParaViewHelpId, aToolId );

  // --- Toolbar "Selection Controls"

  mySelectionControlsTb = createTool( tr("TOOL_SELECTION_CONTROLS") );

  createTool( InteractId, mySelectionControlsTb );
  createTool( SelectCellsOnId, mySelectionControlsTb );
  createTool( SelectPointsOnId, mySelectionControlsTb );
  createTool( SelectCellsThroughId, mySelectionControlsTb );
  createTool( SelectPointsThroughId, mySelectionControlsTb );
  createTool( SelectBlockId, mySelectionControlsTb );

  // --- Toolbar "Undo/Redo Controls"

  aToolId = createTool( tr("TOOL_UNDOREDO_CONTROLS") );

  createTool( UndoId, aToolId );
  createTool( RedoId, aToolId );

  // --- Toolbar "VCR Controls"

  aToolId = createTool( tr("TOOL_VCR_CONTROLS") );

  createTool( FirstFrameId, aToolId );
  createTool( PreviousFrameId, aToolId );
  createTool( PlayId, aToolId );
  createTool( NextFrameId, aToolId );
  createTool( LastFrameId, aToolId );
  createTool( LoopId, aToolId );

  pqProgressManager* progress_manager = pqApplicationCore::instance()->getProgressManager();
  progress_manager->addNonBlockableObject( toolMgr()->toolBar( aToolId ) );

  // --- Toolbar "Active Variable Controls"

  aToolId = createTool( tr("TOOL_ACTIVE_VARIABLE_CONTROLS") );
  QToolBar* aTB = toolMgr()->toolBar( aToolId );
  
  createTool( ShowColorLegendId, aToolId );

  // To set up the color map tool buttons.
  pqColorScaleToolbar* colorScale = Implementation->Core.getColorScaleEditorManager();

  // Edit Color Map
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_EDIT_COLOR_MAP"), false );
  aQtxAction = new QtxAction( tr("TOP_EDIT_COLOR_MAP"), QIcon(aPixmap), 
			      tr("MEN_EDIT_COLOR_MAP"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_EDIT_COLOR_MAP" ) );
  registerAction( EditColorMapId, aQtxAction );
  connect( &Implementation->Core, SIGNAL( enableVariableToolbar(bool) ),
	   aQtxAction,            SLOT( setEnabled(bool) ) );
  colorScale->setColorAction( aQtxAction ); /// !!!
  createTool( EditColorMapId, aToolId );

  // Reset Range
  aPixmap = resMgr->loadPixmap( "ParaView", tr("ICON_RESET_RANGE"), false );
  aQtxAction = new QtxAction( tr("TOP_RESET_RANGE"), QIcon(aPixmap), 
			      tr("MEN_RESET_RANGE"), 0, desk );
  aQtxAction->setStatusTip( tr( "PRP_APP_RESET_RANGE" ) );
  registerAction( ResetRangeId, aQtxAction );
  connect( &Implementation->Core, SIGNAL( enableVariableToolbar(bool) ),
	   aQtxAction,            SLOT( setEnabled(bool) ) );
  colorScale->setRescaleAction( aQtxAction ); /// !!!
  createTool( ResetRangeId, aToolId );

  Implementation->Core.setupVariableToolbar(aTB);
  QList<QAction*> anActns = aTB->actions();
  for (int i = 0; i < anActns.size(); ++i)
    if ( anActns.at(i) != action(ShowColorLegendId) ) {
      createTool( anActns.at(i), aToolId );
      connect( &Implementation->Core, SIGNAL( enableVariableToolbar(bool) ),
	       anActns.at(i),         SLOT( setEnabled(bool) ) );
    }
  
  // --- Toolbar "Representation"

  aToolId = createTool( tr("TOOL_REPRESENTATION") );
  aTB = toolMgr()->toolBar( aToolId );

  Implementation->Core.setupRepresentationToolbar(aTB);
  anActns = aTB->actions();
  for (int i = 0; i < anActns.size(); ++i) {
    createTool( anActns.at(i), aToolId );
    connect( &Implementation->Core, SIGNAL( enableVariableToolbar(bool) ),
	     anActns.at(i),         SLOT( setEnabled(bool) ) );
  }

  // --- Toolbar "Camera Controls"

  aToolId = createTool( tr("TOOL_CAMERA_CONTROLS") );
  
  createTool( ResetCameraId, aToolId );
  createTool( PositiveXId, aToolId );
  createTool( NegativeXId, aToolId );
  createTool( PositiveYId, aToolId );
  createTool( NegativeYId, aToolId );
  createTool( PositiveZId, aToolId );
  createTool( NegativeZId, aToolId );

  // --- Toolbar "Center Axes Controls"

  aToolId = createTool( tr("TOOL_CENTER_AXES_CONTROLS") );

  createTool( ShowCenterId, aToolId );
  createTool( ResetCenterId, aToolId );
  createTool( PickCenterId, aToolId );

  // --- Toolbar "Common Filters"

  aToolId = createTool( tr("TOOL_COMMON_FILTERS") );
  aTB = toolMgr()->toolBar( aToolId );

  Implementation->Core.setupCommonFiltersToolbar(aTB);
  anActns = aTB->actions();
  for (int i = 0; i < anActns.size(); ++i)
    createTool( anActns.at(i), aToolId );
 
  // --- Toolbar "Lookmarks"

  aToolId = createTool( tr("TOOL_LOOKMARKS") );
  aTB = toolMgr()->toolBar(aToolId);

  aTB->setContextMenuPolicy(Qt::CustomContextMenu);
  aTB->setOrientation(Qt::Vertical);
  aTB->setAllowedAreas(Qt::RightToolBarArea);
  toolMgr()->mainWindow()->addToolBar(Qt::RightToolBarArea,aTB);
  Implementation->Core.setupLookmarkToolbar(aTB);
}

/*!
  \brief Returns QMenu by its id.
*/
QMenu* PVGUI_Module::getMenu( const int id )
{
  QMenu* res = 0;
  LightApp_Application* anApp = getApp();
  SUIT_Desktop* desk = anApp->desktop();
  if ( desk ){
    QtxActionMenuMgr* menuMgr = desk->menuMgr();
    res = menuMgr->findMenu( id );
  }
  return res;
}
