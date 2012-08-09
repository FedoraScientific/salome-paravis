// Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

#ifndef _PVGUI_OutputWindowAdapter_h
#define _PVGUI_OutputWindowAdapter_h

#include <vtkOutputWindow.h>

/*!
vtkOutputWindow implementation that puts VTK output messages to SALOME log window.

To use, create an instance of PVGUI_OutputWindowAdapter and pass it to the
vtkOutputWindow::setInstance() static method.

This class is based on pqOutputWindow ParaView class.
*/
class PVGUI_OutputWindowAdapter : public vtkOutputWindow
{
public:
  static PVGUI_OutputWindowAdapter *New();
  vtkTypeRevisionMacro(PVGUI_OutputWindowAdapter, vtkOutputWindow);

  //! Returns the number of text messages received
  const unsigned int getTextCount();
  //! Returns the number of error messages received
  const unsigned int getErrorCount();
  //! Returns the number of warning messages received
  const unsigned int getWarningCount();
  //! Returns the number of generic warning messages received
  const unsigned int getGenericWarningCount();

private:
  PVGUI_OutputWindowAdapter();
  PVGUI_OutputWindowAdapter(const PVGUI_OutputWindowAdapter&);
  PVGUI_OutputWindowAdapter& operator=(const PVGUI_OutputWindowAdapter&);
  ~PVGUI_OutputWindowAdapter();

  unsigned int TextCount;
  unsigned int ErrorCount;
  unsigned int WarningCount;
  unsigned int GenericWarningCount;

  virtual void DisplayText(const char*);
  virtual void DisplayErrorText(const char*);
  virtual void DisplayWarningText(const char*);
  virtual void DisplayGenericWarningText(const char*);
};

#endif // !_PVGUI_OutputWindowAdapter_h
