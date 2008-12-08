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
// File   : PVGUI_Module.h
// Author : Sergey ANIKIN
//

/*!
  \mainpage

  Insert the content of introduction page here.

*/

#ifndef PVGUI_Module_H
#define PVGUI_Module_H

#include <LightApp_Module.h>

class QMenu;
class QDockWidget;
class LightApp_Selection;
class LightApp_SelectionMgr;
class PVGUI_ProcessModuleHelper;
class vtkPVMain;
class pqOptions;
class pqServer;
class pqActiveServer;
class pqViewManager;

class PVGUI_Module : public LightApp_Module
{
  Q_OBJECT
   
  //! Menu actions
  enum { // Menu "File"
         OpenFileId,

	 LoadStateId,
	 SaveStateId,

	 SaveDataId,
	 SaveScreenshotId,
	 ExportId,

	 SaveAnimationId,
	 SaveGeometryId,

	 ConnectId,
	 DisconnectId,

	 // Menu "Edit"
	 UndoId,
	 RedoId,

	 CameraUndoId,
	 CameraRedoId,

	 ChangeInputId,
	 DeleteId,
	 DeleteAllId,

	 InteractId,
	 SelectCellsOnId,
	 SelectPointsOnId,
	 SelectCellsThroughId,
	 SelectPointsThroughId,
	 SelectBlockId,

	 SettingsId,
	 ViewSettingsId,

	 // Menu "View"
	 ResetCameraId,
	 PositiveXId,
	 NegativeXId,
	 PositiveYId,
	 NegativeYId,
	 PositiveZId,
	 NegativeZId,

	 ShowCenterId,
	 ResetCenterId,
	 PickCenterId,
	 ShowColorLegendId,
	 EditColorMapId,
	 ResetRangeId,

	 AnimationInspectorId,
	 AnimationViewId,
	 ComparativeViewInspectorId,
	 SelectionInspectorId,
	 LookmarkBrowserId,
	 LookmarkInspectorId,
	 ObjectInspectorId,
	 PipelineBrowserId,
	 StatisticsViewId,

	 // Menu "Sources"
	 // TODO...

	 // Menu "Filters"
	 // TODO...

	 // Menu "Animation"
	 FirstFrameId,
	 PreviousFrameId,
	 PlayId,
	 NextFrameId,
	 LastFrameId,
	 LoopId,

	 // Menu "Tools" 
	 CreateCustomFilterId,
	 ManageCustomFiltersId,
	 CreateLookmarkId,
	 ManageLinksId,
	 AddCameraLinkId,
	 ManagePluginsExtensionsId,
	 DumpWidgetNamesId,
	 RecordTestId,
	 RecordTestScreenshotId,
	 PlayTestId,
	 MaxWindowSizeId,
	 TimerLogId,
	 OutputWindowId,
	 PythonShellId,

	 // Menu "Help" 
	 AboutParaViewId,
	 ParaViewHelpId,
	 EnableTooltipsId

         /*
         lgLoadFile = 931,   //!< load text file
	 lgSaveFile,         //!< save text file
	 lgDisplayLine,      //!< display selected line
	 lgEraseLine,        //!< erase selected line
	 lgEditLine,         //!< edit selected line
	 lgAddLine,          //!< insert new line
	 lgDelLine,          //!< delete selected line
	 lgClear             //!< clear all contents
	 */
  };

public:
  PVGUI_Module();
  ~PVGUI_Module();

  virtual void           initialize( CAM_Application* );
  virtual void           windows( QMap<int, int>& ) const;

  //virtual LightApp_Selection* createSelection() const;

  pqViewManager*         getMultiViewManager() const;

  virtual bool           eventFilter( QObject*, QEvent* );

protected:
  //virtual CAM_DataModel* createDataModel();

private:
  //! Initialize ParaView if not yet done (once per session)
  static bool            pvInit();  
  //! Shutdown ParaView, should be called on application exit
  static void            pvShutdown();   
 
  //! Create actions for ParaView GUI operations
  void                   pvCreateActions();

  //! Create menus for ParaView GUI operations
  //! duplicating menus in pqMainWindow ParaView class
  void                   pvCreateMenus();

  //! Create toolbars for ParaView GUI operations
  //! duplicating toolbars in pqMainWindow ParaView class
  void                   pvCreateToolBars();

  //! Create dock widgets for ParaView widgets
  void                   setupDockWidgets();

  //! Create dock widgets context menus
  void                   setupDockWidgetsContextMenu();

  //! Save states of dockable ParaView widgets
  void                   saveDockWidgetsState();

  //! Restore states of dockable ParaView widgets
  void                   restoreDockWidgetsState();

  //! Shows or hides ParaView view window
  void                   showView( bool );    

  //! Returns QMenu object for a given menu id
  QMenu*                 getMenu( const int );

private slots:
  void onUndoLabel( const QString& );
  void onRedoLabel( const QString& );
  
  void onCameraUndoLabel( const QString& );
  void onCameraRedoLabel( const QString& );

  void onDeleteAll();

  void onSelectionModeChanged( int mode );

  void onShowCenterAxisChanged( bool );

  void setTimeRanges( double, double );

  void onPlaying( bool );
  
  void onAddCameraLink();
  
  void onHelpAbout();

  void showHelpForProxy( const QString& proxy );
  void makeAssistant();
  void assistantError( const QString& err );
  
  void onPreAccept();
  void onPostAccept();
  void endWaitCursor();

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );

private:
  class pqImplementation;
  pqImplementation*      Implementation;

  int                    mySelectionControlsTb;
  int                    mySourcesMenuId;
  int                    myFiltersMenuId;

  QList<QDockWidget*>    myDockWidgets;
};

#endif // PVGUI_Module_H
