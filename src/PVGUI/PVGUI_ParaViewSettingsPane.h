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

class QAbstractButton;
class vtkSMProperty;
class QShowEvent;
class QHideEvent;

/**!
 * Almost identical copy/paste of what is found in
 *    <PARAVIEW>/Qt/Components/pqSettingsDialog.h
 * This is (almost) the same, except for the inheritance to QtxUserDefinedContent providing the link
 * to SALOME preference mechanism.
 * The UI content of pqSettingsDialog is put in a (PARAVIS) widget called pqCustomSettingsWidget.
 */
class PVGUI_ParaViewSettingsPane : public QtxUserDefinedContent
{
  Q_OBJECT

public:
  PVGUI_ParaViewSettingsPane(QWidget *parent=0);
  virtual ~PVGUI_ParaViewSettingsPane();

  /// Implementation of QtxUserDefinedContent interface
  virtual void store(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);
  virtual void retrieve(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);

  signals:
    void accepted(); // emulate the initial QDialogBox signal since some stuff are connected to it.

protected:
    virtual void showEvent(QShowEvent * ev);
    virtual void hideEvent(QHideEvent * ev);

  // ---- From now on, this the same interface as in pqSettingsDialog
  private slots:
    void clicked(QAbstractButton*);
    void onAccepted();
    void onRejected();
    void onRestoreDefaults();

    void onTabIndexChanged(int index);
    void onChangeAvailable();
    void showRestartRequiredMessage();

    void filterPanelWidgets();

  signals:
    void filterWidgets(bool showAdvanced, const QString& text);

  private:
    void saveInQSettings(const char* key, vtkSMProperty* smproperty);

  private:
    Q_DISABLE_COPY(PVGUI_ParaViewSettingsPane);
    class pqInternals;
    pqInternals* Internals;

    /// Set to true if a setting that requires a restart to take effect
    /// is modified. Made static to persist across instantiations of
    /// this class.
    static bool ShowRestartRequired;
};

#endif
