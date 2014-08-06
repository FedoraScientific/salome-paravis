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
// Author : Anthony Geay

#ifndef _pqExtractCellTypePanel_h
#define _pqExtractCellTypePanel_h

#include "pqNamedObjectPanel.h"

#include <map>

class pqTreeWidgetItemObject;

class vtkSMProperty;
class vtkMutableDirectedGraph;

class pqExtractCellTypePanel : public pqNamedObjectPanel
{
Q_OBJECT
  typedef pqNamedObjectPanel Superclass;
public:
  pqExtractCellTypePanel(pqProxy* proxy, QWidget* p = NULL);
  ~pqExtractCellTypePanel();
protected slots:
  void updateSIL();
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();
  class pqUI;
  pqUI* UI;
  std::vector<pqTreeWidgetItemObject *> _all_lev4;
};

#endif
