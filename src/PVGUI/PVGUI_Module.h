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


#ifndef PVGUI_Module_H
#define PVGUI_Module_H

#include <SalomeApp_Module.h>

#include <ostream>

class QMenu;
class QDockWidget;
class QToolBar;
class vtkPVMain;
class pqOptions;
class pqServer;
class pqViewManager;
class pqMainWindowCore;

class PVGUI_Module : public SalomeApp_Module
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
   IgnoreTimeId, 
	 DeleteId,
	 DeleteAllId,

	 //InteractId,
	 //SelectCellsOnId,
	 //SelectPointsOnId,
	 //SelectCellsThroughId,
	 //SelectPointsThroughId,
	 //SelectBlockId,

	 SettingsId,
	 ViewSettingsId,

	 // Menu "View"
   FullScreenId, 
// 	 ResetCameraId,
// 	 PositiveXId,
// 	 NegativeXId,
// 	 PositiveYId,
// 	 NegativeYId,
// 	 PositiveZId,
// 	 NegativeZId,
// 
// 	 ShowCenterId,
// 	 ResetCenterId,
// 	 PickCenterId,
// 	 ShowColorLegendId,
// 	 EditColorMapId,
// 	 ResetRangeId,
// 
// 	 AnimationInspectorId,
// 	 AnimationViewId,
// 	 ComparativeViewInspectorId,
// 	 SelectionInspectorId,
// 	 LookmarkBrowserId,
// 	 LookmarkInspectorId,
// 	 ObjectInspectorId,
// 	 PipelineBrowserId,
// 	 StatisticsViewId,

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
  };

public:
  PVGUI_Module();
  ~PVGUI_Module();

  virtual void           initialize( CAM_Application* );
  virtual void           windows( QMap<int, int>& ) const;

  pqViewManager*         getMultiViewManager() const;

  virtual QString engineIOR() const;

  /*! Compares the contents of the window with the given reference image,
   * returns true if they "match" within some tolerance
   */
  /*bool                   compareView( const QString& ReferenceImage, double Threshold,
    std::ostream& Output, const QString& TempDirectory );*/

  void openFile(const char* theName);
  void saveParaviewState(const char* theFileName);
  void loadParaviewState(const char* theFileName);

  QString getTraceString();
  void saveTrace(const char* theName);

  pqServer* getActiveServer();

  virtual void createPreferences();

public slots:
  void onImportFromVisu(QString theEntry);

private:
  //! Initialize ParaView if not yet done (once per session)
  static bool            pvInit();  
 
  //! Create actions for ParaView GUI operations
  void                   pvCreateActions();

  //! Create menus for ParaView GUI operations duplicating menus in pqMainWindow ParaView class
  void                   pvCreateMenus();

  //! Create toolbars for ParaView GUI operations duplicating toolbars in pqMainWindow ParaView class
  void                   pvCreateToolBars();

  //! Create dock widgets for ParaView widgets
  void                   setupDockWidgets();

  //! Save states of dockable ParaView widgets
  void                   saveDockWidgetsState();

  //! Restore states of dockable ParaView widgets
  void                   restoreDockWidgetsState();

  //! Shows or hides ParaView view window
  void                   showView( bool );    

  //! Returns QMenu object for a given menu id
  QMenu*                 getMenu( const int );

  void                   deleteTemporaryFiles();
  
  QList<QToolBar*>       getParaViewToolbars();

private slots:

  void showHelpForProxy( const QString& proxy );
  
  void onPreAccept();
  void onPostAccept();
  void endWaitCursor();

  void activateTrace();

  void buildToolbarsMenu();

  void showParaViewHelp();
  void showHelp(const QString& url);

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );
  virtual void           onApplicationClosed( SUIT_Application* );

private:
  class pqImplementation;
  pqImplementation*      Implementation;

  int                    mySelectionControlsTb;
  int                    mySourcesMenuId;
  int                    myFiltersMenuId;
  int                    myToolbarsMenuId;
  int                    myMacrosMenuId;

  QList<QDockWidget*>    myDockWidgets;

  QStringList            myTemporaryFiles;

  QMap<QToolBar*, bool>  myToolbarState;

  QtMsgHandler           myOldMsgHandler;
};

#endif // PVGUI_Module_H
