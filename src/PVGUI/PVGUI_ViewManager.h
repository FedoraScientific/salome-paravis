// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef PVGUI_VIEWMANAGER_H
#define PVGUI_VIEWMANAGER_H

#include <SUIT_ViewManager.h>

class SUIT_Desktop;
class SUIT_Study;
class pqViewManager;

class PVGUI_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  PVGUI_ViewManager( SUIT_Study*, SUIT_Desktop* );
  ~PVGUI_ViewManager();

  pqViewManager*     getMultiViewManager();
};

#endif
