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

#include <LightApp_Module.h>

class QMenu;
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
  /*enum { 
    lgLoadFile = 931,   //!< load text file
    lgSaveFile,         //!< save text file
    lgDisplayLine,      //!< display selected line
    lgEraseLine,        //!< erase selected line
    lgEditLine,         //!< edit selected line
    lgAddLine,          //!< insert new line
    lgDelLine,          //!< delete selected line
    lgClear             //!< clear all contents
    };*/

public:
  PVGUI_Module();
  ~PVGUI_Module();

  virtual void           initialize( CAM_Application* );
  virtual void           windows( QMap<int, int>& ) const;

  //virtual LightApp_Selection* createSelection() const;

  pqServer*              getActiveServer() const;
  pqViewManager*         getMultiViewManager() const;

protected:
  //virtual CAM_DataModel* createDataModel();

private:
  //! Initialize ParaView if not yet done (once per session)
  static bool            pvInit();  
  //! Shutdown ParaView, should be called on application exit
  static void            pvShutdown();   
 
  //! Create actions for ParaView GUI operations
  //! duplicating menus and toolbars in pqMainWindow ParaView class
  void                   pvCreateActions();  

  //! Shows or hides ParaView view window
  void                   showView( bool );         

  void                   makeDefaultConnectionIfNoneExists();

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );

private:
  static vtkPVMain*                 myPVMain;
  static pqOptions*                 myPVOptions;
  static PVGUI_ProcessModuleHelper* myPVHelper;

  //! pqMainWindowCore stuff
  pqActiveServer*                   myActiveServer;
};

#endif // PVGUI_Module_H
