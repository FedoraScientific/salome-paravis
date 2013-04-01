// PARAVIS : ParaView wrapper SALOME module
//
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

  /// \brief
  ///   Gets whether or not there are changes to apply.
  /// \return
  ///   True if there are changes to apply.
  //bool isApplyNeeded() const;

  /// \brief
  ///   Sets whether or not there are changes to apply.
  /// \param applyNeeded True if there are changes to apply.
  //void setApplyNeeded(bool applyNeeded);

  /// \brief
  ///   Adds a page to the options dialog.
  ///
  /// When the options object is a page container, the path parameter
  /// becomes the path prefix for the container pages.
  ///
  /// \param path The name hierarchy for the options page.
  /// \param options The options page.
  void addOptions(const QString &path, pqOptionsPage *options);

  /// \brief
  ///   Adds a container to the options dialog.
  ///
  /// Each page listed for the container is added to the root of the
  /// selection tree.
  ///
  /// \param options The options container to add.
  void addOptions(pqOptionsContainer *options);

  /// \brief
  ///   Removes the options page from the options dialog.
  ///
  /// The page name is removed from the selection tree. If the page
  /// is an options container, all the names are removed.
  ///
  /// \param options The options page/container to remove.
  void removeOptions(pqOptionsPage *options);

  /// Implementation of QtxUserDefinedContent interface
  virtual void store(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);
  virtual void retrieve(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref);


public slots:
  /// \brief
  ///   Sets the current options page.
  /// \param path The name of the options page to show.
  void setCurrentPage(const QString &path);

  /// Calls each page to apply any changes.
  void applyChanges();

  /// Calls each page to reset any changes.
  void resetChanges();

signals:
  /// Emitted before the option changes are applied.
  void aboutToApplyChanges();

  /// Emitted after the option changes have been applied.
  void appliedChanges();

protected slots:
  void pluginLoaded(QObject* plugin_interface);

private slots:
  /// Changes the current page to match the user selection.
  void changeCurrentPage();

private:
  OptionsDialogForm *Form; /// Stores the form and class data.
};

#endif
