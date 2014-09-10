// PARAVIS : ParaView wrapper SALOME module
//
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
// File   : PVGUI_ParaViewSettingsPane.h
// Author : Vitaly Smetannikov
//


#ifndef PVGUI_ParaViewSettingsPane_H
#define PVGUI_ParaViewSettingsPane_H

#include <QtxPagePrefMgr.h>

#include <QWidget>

#include "vtkPVConfig.h"

class pqOptionsContainer;
class OptionsDialogForm;
class pqOptionsPage;
class QString;


class PVGUI_ParaViewSettingsPane : public QtxUserDefinedContent
{
  Q_OBJECT

public:
  PVGUI_ParaViewSettingsPane(QWidget *parent=0);
  virtual ~PVGUI_ParaViewSettingsPane();

  /// Implementation of QtxUserDefinedContent interface
  virtual void store(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);
  virtual void retrieve(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);


public slots:
  /// Calls each page to apply any changes.
  void applyChanges();

  /// Calls each page to reset any changes.
  void resetChanges();

  void onRequestParaviewSettings();

signals:
  /// Emitted before the option changes are applied.
  void aboutToApplyChanges();

  /// Emitted after the option changes have been applied.
  void appliedChanges();

private:
  OptionsDialogForm *Form; /// Stores the form and class data.
};

#endif
