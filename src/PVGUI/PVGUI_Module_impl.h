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

#include <QAssistantClient>
#include <QPointer>

#include <pqMainWindowCore.h>
#include <pqRecentFilesMenu.h>

//////////////////////////////////////////////////////////////////////////////
// PVGUI_Module::pqImplementation

class PVGUI_Module::pqImplementation
{
 public:
  pqImplementation(QWidget* parent) :
    AssistantClient(0),
    Core(parent),
    RecentFilesMenu(0)
  {
  }
  
  ~pqImplementation()
  {
    if(this->AssistantClient) {
      this->AssistantClient->closeAssistant();
      delete this->AssistantClient;
    }
  }
  
  QPointer<QAssistantClient> AssistantClient;
  pqMainWindowCore Core;
  pqRecentFilesMenu* RecentFilesMenu;
  QPointer<pqServer> ActiveServer;
  QString DocumentationDir;
  
  static vtkPVMain* myPVMain;
  static pqOptions* myPVOptions;
  static PVGUI_ProcessModuleHelper* myPVHelper;
};

#endif // PVGUI_Module_impl_H
