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
// File   : PVGUI_Module.h
// Author : Sergey ANIKIN
//


#ifndef PVGUI_Module_H
#define PVGUI_Module_H

#include <SalomeApp_Module.h>
#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOMEDS)
#ifndef PARAVIS_WITH_FULL_CORBA
#    include CORBA_SERVER_HEADER(PARAVIS_Gen)
#endif

#include <ostream>
#include <vtkType.h>
#include <QTimer>

#include <pqVariableType.h>

class QMenu;
class QDockWidget;
class QToolBar;
class vtkPVMain;
class pqOptions;
class pqServer;
class pqMainWindowCore;
class vtkEventQtSlotConnect;
class pqPythonScriptEditor;
class pqPVApplicationCore;
class pqDataRepresentation;
class pqRepresentation;
class PVViewer_GUIElements;

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

     FindDataId,
     ChangeInputId,
     IgnoreTimeId,
     DeleteId,
     DeleteAllId,

     SettingsId,
     ViewSettingsId,

     // Menu "View"
     FullScreenId,

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
     CustomWindowSizeId,
     TimerLogId,
     OutputWindowId,
     PythonShellId,
     ShowTraceId,
     RestartTraceId,

     // Menu "Help"
     AboutParaViewId,
     ParaViewHelpId,
     EnableTooltipsId,

     // Menu "Window" - "New Window"
     ParaViewNewWindowId,

     // "Save state" ParaVis module root object popup
     SaveStatePopupId,

     // "Add state" and "Reload state" popups
     AddStatePopupId,
     CleanAndAddStatePopupId,

     // "Rename" and "Delete" popups (Object Browser)
     ParaVisRenameId,
     ParaVisDeleteId
  };

public:
  PVGUI_Module();
  ~PVGUI_Module();

  virtual void           initialize( CAM_Application* );
  virtual void           windows( QMap<int, int>& ) const;

  virtual QString engineIOR() const;

  void openFile(const char* theName);
  void executeScript(const char *script);
  void saveParaviewState(const char* theFileName);
  void loadParaviewState(const char* theFileName);
  void clearParaviewState();

  QString getTraceString();
  void startTrace();
  void stopTrace();
  void saveTrace(const char* theName);

  pqServer* getActiveServer();

  virtual void createPreferences();

  virtual void contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle);

  inline static PARAVIS_ORB::PARAVIS_Gen_var GetEngine();
  inline static pqPVApplicationCore * GetPVApplication();

public slots:
  //void onImportFromVisu(QString theEntry);

private:
  void deleteTemporaryFiles();
 
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
  
  //! Discover help project files from the resources.
  QString getHelpFileName();

  //! Create actions for ParaViS
  void createActions();

  //! Create menus for ParaViS
  void createMenus();

  //! Load selected state
  void loadSelectedState(bool toClear);

  //! Get list of embedded macros files
  QStringList getEmbeddedMacrosList();

  //! update macros state
  void updateMacros();

  //! store visibility of the common dockable windows (OB, PyConsole, ... etc.)
  void storeCommonWindowsState();

  //! restore visibility of the common dockable windows (OB, PyConsole, ... etc.)
  void restoreCommonWindowsState();

private slots:

  void showHelpForProxy( const QString&, const QString& );
  
  void onPreAccept();
  void onPostAccept();
  void endWaitCursor();

  //  void buildToolbarsMenu();

  //void showParaViewHelp();
  //void showHelp(const QString& url);

  void onDataRepresentationUpdated();

  void onStartProgress();
  void onEndProgress();
  void onShowTrace();
  void onRestartTrace();

  void onNewParaViewWindow();

  void onSaveMultiState();
  void onAddState();
  void onCleanAddState();

  void onRename();
  void onDelete();

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );
  virtual void           onApplicationClosed( SUIT_Application* );
  virtual void           studyClosed( SUIT_Study* );

protected slots:
  virtual void           onModelOpened();
  virtual void           onPushTraceTimer();
  virtual void           onInitTimer();

private:
  int                    mySelectionControlsTb;
  int                    mySourcesMenuId;
  int                    myFiltersMenuId;
  int                    myToolbarsMenuId;
  int                    myMacrosMenuId;
  int                    myRecentMenuId;
  
  typedef QMap<QWidget*, bool> WgMap;
  WgMap                  myDockWidgets;
  WgMap                  myToolbars;
  WgMap                  myToolbarBreaks;
  QList<QMenu*>          myMenus;

  typedef QMap<int, bool> DockWindowMap;         
  DockWindowMap           myCommonMap; 

  QStringList            myTemporaryFiles;

  QtMsgHandler           myOldMsgHandler;

  vtkEventQtSlotConnect *VTKConnect;

  pqPythonScriptEditor* myTraceWindow;

  int myStateCounter;

  //! Single shot timer used to connect to the PVServer, and start the trace.
  QTimer             * myInitTimer;

  //! Timer used to regularly push the Python trace to the engine.
  QTimer             * myPushTraceTimer;

  PVViewer_GUIElements * myGuiElements;
};

#endif // PVGUI_Module_H
