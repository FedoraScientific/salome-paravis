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

#ifndef _pqTableReaderPanel_h
#define _pqTableReaderPanel_h

#include "ui_TableReaderPanel.h"

#include "pqNamedObjectPanel.h"

class QSpinBox;

class pqTableReaderPanel: public pqNamedObjectPanel
{
Q_OBJECT
  typedef pqNamedObjectPanel Superclass;
public:
  // Constructor
  pqTableReaderPanel(pqProxy* proxy, QWidget* p = NULL);

  // Destructor
  ~pqTableReaderPanel();

protected slots:
  void onDelimiterChanged(const QString& value);
  void onCurrentTableChanged(int currentTableIndex);

protected:
  // Populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

  // Update the list with available table names
  void updateAvailableTables(const bool keepCurrent);

  class pqUI;
  pqUI* UI;
};

#endif

