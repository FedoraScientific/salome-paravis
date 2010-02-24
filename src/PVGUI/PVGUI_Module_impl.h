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
// File   : PVGUI_Module_impl.h
// Author : Margarita KARPUNINA
//

#ifndef PVGUI_Module_impl_H
#define PVGUI_Module_impl_H

#include "PVGUI_Module.h"
#include "PVGUI_OutputWindowAdapter.h"

//#include <QAssistantClient>
#include <pqHelpWindow.h>
#include <QPointer>

#include <pqRecentFilesMenu.h>
#include <pqPVApplicationCore.h>
#include <pqPQLookupTableManager.h>
#include <pqActiveObjects.h>


//////////////////////////////////////////////////////////////////////////////
// PVGUI_Module::pqImplementation

class PVGUI_Module::pqImplementation
{
 public:
  pqImplementation(QWidget* parent):
    RecentFilesMenu(0)
  { }
  
  ~pqImplementation()
  { }
  
  pqRecentFilesMenu* RecentFilesMenu;
  static pqApplicationCore* Core;
  static PVGUI_OutputWindowAdapter* OutputWindowAdapter;
  static QPointer<pqHelpWindow> helpWindow;
};

#endif // PVGUI_Module_impl_H
