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
#include <PVGUI_ViewManager.h>
#include <PVGUI_ViewModel.h>
#include <PVGUI_ViewWindow.h>
#include <pqApplicationCore.h>
#include <pqTabbedMultiViewWidget.h>

/*!
  Constructor
*/
PVGUI_ViewManager::PVGUI_ViewManager( SUIT_Study* study, SUIT_Desktop* desk ) 
: SUIT_ViewManager( study, desk, new PVGUI_Viewer() )
{
  setTitle( tr( "PARAVIEW_VIEW_TITLE" ) );
}

/*!
  \brief Returns the ParaView multi-view manager for the active view window
*/
pqTabbedMultiViewWidget* PVGUI_ViewManager::getMultiViewManager()
{
  return qobject_cast<pqTabbedMultiViewWidget*>(pqApplicationCore::instance()->manager("MULTIVIEW_WIDGET"));
}
