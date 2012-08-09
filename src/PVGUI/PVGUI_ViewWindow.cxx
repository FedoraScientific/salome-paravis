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
// File   : PVGUI_ViewWindow.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PVGUI_ViewWindow.h"

#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>

#include <pqTabbedMultiViewWidget.h>
#include <pqApplicationCore.h>

/*!
  \class PVGUI_ViewWindow
  \brief PVGUI view window.
*/

/*!
  \brief Constructor.
  \param theDesktop parent desktop window
  \param theModel plt2d view model
*/
PVGUI_ViewWindow::PVGUI_ViewWindow( SUIT_Desktop* theDesktop, PVGUI_Viewer* theModel )
  : SUIT_ViewWindow( theDesktop ), myPVMgr( 0 )
{
  myModel = theModel;
  myPVMgr = qobject_cast<pqTabbedMultiViewWidget*>(pqApplicationCore::instance()->manager("MULTIVIEW_WIDGET"));
  if (myPVMgr) {
    myPVMgr->setParent( this );
    // This is mandatory, see setParent() method in Qt 4 documentation
    myPVMgr->show();
    setCentralWidget( myPVMgr );
  } else
    qDebug("No multiViewManager defined");
}

/*!
  \brief Destructor.
  As pqViewManager persists through the whole session,
  the destructor first removes it from the children of this PVGUI_ViewWindow
  to prevent its unexpected deletion.
*/
PVGUI_ViewWindow::~PVGUI_ViewWindow()
{
  if ( myPVMgr ) {
    myPVMgr->setParent( 0 );
    myPVMgr->hide();
    myPVMgr = 0;
    setCentralWidget( 0 );
  }
}

/*!
  \brief Get the visual parameters of the view window.
  \return visual parameters of this view window formatted to the string
*/
QString PVGUI_ViewWindow::getVisualParameters()
{
  return SUIT_ViewWindow::getVisualParameters();
}

/*!
  \brief Restore visual parameters of the view window from the formated string
  \param parameters view window visual parameters
*/
void PVGUI_ViewWindow::setVisualParameters( const QString& parameters )
{
  SUIT_ViewWindow::setVisualParameters( parameters );
}


/*!
  \brief Returns the ParaView multi-view manager previously set with setMultiViewManager()
*/
pqTabbedMultiViewWidget* PVGUI_ViewWindow::getMultiViewManager() const
{
  return myPVMgr;
}
