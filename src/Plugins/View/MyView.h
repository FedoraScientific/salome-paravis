// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

#ifndef _MyView_h
#define _MyView_h

#include "pqView.h"
#include <QMap>
#include <QColor>
class QLabel;

/// a simple view that shows a QLabel with the display's name in the view
class MyView : public pqView
{
  Q_OBJECT
public:
    /// constructor takes a bunch of init stuff and must have this signature to 
    /// satisfy pqView
  MyView(const QString& viewtypemodule, 
         const QString& group, 
         const QString& name, 
         vtkSMViewProxy* viewmodule, 
         pqServer* server, 
         QObject* p);
  ~MyView();

  /// don't support save images
  bool saveImage(int, int, const QString& ) { return false; }
  vtkImageData* captureImage(int) { return NULL; }
  vtkImageData* captureImage(const QSize&) { return NULL; }

  /// return the QWidget to give to ParaView's view manager
  QWidget* getWidget();

  /// returns whether this view can display the given source
  bool canDisplay(pqOutputPort* opPort) const;

  /// set the background color of this view
  void setBackground(const QColor& col);
  QColor background() const;

protected slots:
  /// helper slots to create labels
  void onRepresentationAdded(pqRepresentation*);
  void onRepresentationRemoved(pqRepresentation*);

protected:

  QWidget* MyWidget;
  QMap<pqRepresentation*, QLabel*> Labels;

};

#endif // _MyView_h

