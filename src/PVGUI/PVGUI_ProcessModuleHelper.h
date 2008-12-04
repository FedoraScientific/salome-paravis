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
// File   : PVGUI_ProcessModuleHelper.h
// Author : Sergey ANIKIN
//

#ifndef PVGUI_ProcessModuleHelper_H
#define PVGUI_ProcessModuleHelper_H

#include <pqProcessModuleGUIHelper.h>

/*! 
  \class PVGUI_ProcessModuleHelper
  The key element of ParaView integration into SALOME GUI.
  Sub-classes pqProcessModuleGUIHelper with the following main features:
  \li No main window is created by this class
  \li Qt event loop is run outside this class
  \li ParaView messages are redirected to SALOME GUI message console
  \li Client-server architecture is intialized and used as in any ParaView client
 */
class PVGUI_ProcessModuleHelper : public pqProcessModuleGUIHelper
{
public:
  static PVGUI_ProcessModuleHelper* New();
  vtkTypeRevisionMacro(PVGUI_ProcessModuleHelper, pqProcessModuleGUIHelper);
  void PrintSelf(ostream& os, vtkIndent indent);

  //! Compares the contents of the window with the given reference image, returns true iff they "match" within some tolerance
  virtual  bool compareView(const QString& ReferenceImage, double Threshold, ostream& Output, const QString& TempDirectory);

  //! Redefined to supress showing a default output window, as the messages are redirected
  //! to SALOME GUI message console
  virtual void showOutputWindow();

  //! Stub for main window
  virtual void showWindow();
  //! Stub for main window
  virtual void hideWindow();

protected:
  //! Calls the base implementation and then creates a custom output window adaptor
  virtual int InitializeApplication(int argc, char** argv);

  //! Redefined to prevent QApplication::exec() call
  virtual int appExec();

  //! Redefined to do nothing at the end of RunGUIStart().
  //! FinalizeApplication() should be called when the SALOME module
  //! is destroyed.
  virtual int postAppExec();

  PVGUI_ProcessModuleHelper();
  ~PVGUI_ProcessModuleHelper();

  //! Stub for main window, returns a null pointer
  virtual QWidget* CreateMainWindow();

  //! Called by vtkProcessModule,  but we cannot exit here.
  virtual void ExitApplication();

  //! Returns the number of errors registered in the OutputWindow
  //virtual int ErrorCount();

private:
  PVGUI_ProcessModuleHelper(const PVGUI_ProcessModuleHelper&); // Not implemented.
  void operator=(const PVGUI_ProcessModuleHelper&); // Not implemented.

  class pqImplementation;
  pqImplementation* const Implementation;
};

#endif
