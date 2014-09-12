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
// File   : PVGUI_ParaViewSettingsPane.cxx
// Author : Vitaly Smetannikov
//

#include "PVGUI_ParaViewSettingsPane.h"
#include "ui_pqCustomSettingsWidget.h"

#include <QtxDialog.h>

#include <QString>


#include "pqActiveObjects.h"
#include "pqApplicationCore.h"
#include "pqProxyWidget.h"
#include "pqSearchBox.h"
#include "pqServer.h"
#include "pqSettings.h"
#include "pqUndoStack.h"
#include "vtkNew.h"
#include "vtkPVXMLElement.h"
#include "vtkSmartPointer.h"
#include "vtkSMProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMPropertyIterator.h"
#include "vtkSMProxy.h"
#include "vtkSMProxyIterator.h"
#include "vtkSMSessionProxyManager.h"
#include "vtkSMSettings.h"

#include <QKeyEvent>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QShowEvent>
#include <QHideEvent>

// This class is revisited to hack on the SALOME's Preferences "Default" button
// to ParaView's "Default" behavior.
class PVGUI_ParaViewSettingsPane::pqInternals
{
public:
  pqInternals()
  {
    // Get the containing preference tab to identify when it has focus

  }
  ~pqInternals();

  Ui::CustomSettingsWidget Ui;

  // Map from tab indices to stack widget indices. This is needed because there
  // are more widgets in the stacked widgets than just what we add.
  QMap<int, int> TabToStackedWidgets;
};

bool PVGUI_ParaViewSettingsPane::ShowRestartRequired = false;

//----------------------------------------------------------------------------
PVGUI_ParaViewSettingsPane::PVGUI_ParaViewSettingsPane(QWidget *widgetParent)
  : QtxUserDefinedContent(widgetParent),
    Internals (new PVGUI_ParaViewSettingsPane::pqInternals())
{
  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
  ui.setupUi(this);
  ui.tabBar->setDocumentMode(false);
  ui.tabBar->setDrawBase(false);
  ui.tabBar->setExpanding(false);
  ui.tabBar->setUsesScrollButtons(true);

  // Hide restart message
  ui.restartRequiredLabel->setVisible(PVGUI_ParaViewSettingsPane::ShowRestartRequired);

  QList<vtkSMProxy*> proxies_to_show;

  pqServer* server = pqActiveObjects::instance().activeServer();
  vtkNew<vtkSMProxyIterator> iter;
  iter->SetSession(server->session());
  iter->SetModeToOneGroup();
  for (iter->Begin("settings"); !iter->IsAtEnd(); iter->Next())
    {
    vtkSMProxy* proxy = iter->GetProxy();
    if (proxy)
      {
      proxies_to_show.push_back(proxy);
      }
    }

  // Add color palette.
  if (vtkSMProxy* proxy = server->proxyManager()->GetProxy("global_properties", "ColorPalette"))
    {
    proxies_to_show.push_back(proxy);
    }

  foreach (vtkSMProxy* proxy, proxies_to_show)
    {
    QString proxyName = proxy->GetXMLName();

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QString("ScrollArea%1").arg(proxyName));
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(scrollArea);
    container->setObjectName("Container");
    container->setContentsMargins(6, 0, 6, 0);

    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    pqProxyWidget* widget = new pqProxyWidget(proxy, container);
    widget->setObjectName("ProxyWidget");
    widget->setApplyChangesImmediately(false);
    widget->setView(NULL);

    widget->connect(this, SIGNAL(accepted()), SLOT(apply()));
    widget->connect(this, SIGNAL(rejected()), SLOT(reset()));
    this->connect(widget, SIGNAL(restartRequired()), SLOT(showRestartRequiredMessage()));
    vbox->addWidget(widget);

    QSpacerItem* spacer = new QSpacerItem(0, 0,QSizePolicy::Fixed,
      QSizePolicy::MinimumExpanding);
    vbox->addItem(spacer);

    scrollArea->setWidget(container);
    // show panel widgets
    widget->updatePanel();

    int tabIndex = ui.tabBar->addTab(proxy->GetXMLLabel());
    int stackIndex = ui.stackedWidget->addWidget(scrollArea);
    this->Internals->TabToStackedWidgets[tabIndex] = stackIndex;

    this->connect(widget, SIGNAL(changeAvailable()), SLOT(onChangeAvailable()));
    widget->connect(this, SIGNAL(filterWidgets(bool, QString)), SLOT(filterWidgets(bool, QString)));
    }

  // Disable some buttons to start
//  ui.buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
//  ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

//  this->connect(ui.buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
//                SLOT(onRestoreDefaults()));
//  this->connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(clicked(QAbstractButton*)));
  this->connect(this, SIGNAL(accepted()), SLOT(onAccepted()));
  this->connect(this, SIGNAL(rejected()), SLOT(onRejected()));
  this->connect(ui.tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));

  this->connect(ui.SearchBox, SIGNAL(advancedSearchActivated(bool)), SLOT(filterPanelWidgets()));
  this->connect(ui.SearchBox, SIGNAL(textChanged(QString)), SLOT(filterPanelWidgets()));

  // After all the tabs are set up, select the first
  this->onTabIndexChanged(0);

  this->filterPanelWidgets();
}

//-----------------------------------------------------------------------------
PVGUI_ParaViewSettingsPane::~PVGUI_ParaViewSettingsPane()
{
  delete this->Internals;
  this->Internals = NULL;
}

void PVGUI_ParaViewSettingsPane::store(QtxResourceMgr* , QtxPreferenceMgr* )
{
  emit this->accepted();
}

void PVGUI_ParaViewSettingsPane::retrieve(QtxResourceMgr* , QtxPreferenceMgr* )
{
//  onRestoreDefaults();
}

void PVGUI_ParaViewSettingsPane::showEvent(QShowEvent * ev)
{
//  // Connect SALOME's default button to ParaView's default restore.
//  LightApp_PreferencesDlg * prefDg;
//  QWidget *w = this->parentWidget();
//  // UGLY!!!
//  while (w)
//    {
//      LightApp_PreferencesDlg * prefDg = dynamic_cast<LightApp_PreferencesDlg *>( w );
//      if (prefDg)
//        break;
//      w = w->parentWidget();
//    }
//  if (prefDg)
//    prefDg->connect(btn, SIGNAL(clicked()), this, SLOT(onRestoreDefaults()));
  ev->accept();
}

void PVGUI_ParaViewSettingsPane::hideEvent(QHideEvent * ev)
{
  // Connect SALOME's default button to ParaView's default restore.
  ev->accept();
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::clicked(QAbstractButton *button)
{
//  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
//  QDialogButtonBox::ButtonRole role = ui.buttonBox->buttonRole(button);
//  switch (role)
//    {
//  case QDialogButtonBox::AcceptRole:
//  case QDialogButtonBox::ApplyRole:
//    emit this->accepted();
//    break;
//
//  case QDialogButtonBox::ResetRole:
//  case QDialogButtonBox::RejectRole:
//    emit this->rejected();
//    break;
//  default:
//    break;
//    }
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::onAccepted()
{
  // If there are any properties that needed to save their values in QSettings,
  // do that. Otherwise, save to the vtkSMSettings singleton.
  vtkSMSettings * settings = vtkSMSettings::GetInstance();
  pqServer* server = pqActiveObjects::instance().activeServer();
  vtkNew<vtkSMProxyIterator> iter;
  iter->SetSession(server->session());
  iter->SetModeToOneGroup();
  for (iter->Begin("settings"); !iter->IsAtEnd(); iter->Next())
    {
    vtkSMProxy* proxy = iter->GetProxy();
    settings->SetProxySettings(proxy);
    vtkSmartPointer<vtkSMPropertyIterator> iter2;
    iter2.TakeReference(proxy->NewPropertyIterator());
    for (iter2->Begin(); !iter2->IsAtEnd(); iter2->Next())
      {
      vtkSMProperty* smproperty = iter2->GetProperty();
      if (smproperty && smproperty->GetHints() &&
        smproperty->GetHints()->FindNestedElementByName("SaveInQSettings"))
        {
        QString key = QString("%1.%2").arg(iter->GetKey()).arg(iter2->GetKey());
        this->saveInQSettings(key.toLatin1().data(), smproperty);
        }
      }
    }

  // Save color palette settings
  vtkSMProxy* paletteProxy = server->proxyManager()->GetProxy("global_properties", "ColorPalette");
  if (paletteProxy)
    {
    settings->SetProxySettings(paletteProxy);
    }

  // Disable buttons
//  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
//  ui.buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
//  ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

  // In theory, the above changes are undo-redo able, the only things that's not
  // undo-able is the "serialized" values. Hence we just clear the undo stack.
  CLEAR_UNDO_STACK();
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::onRejected()
{
  // Disable buttons
//  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
//  ui.buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
//  ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::onRestoreDefaults()
{
  pqServer* server = pqActiveObjects::instance().activeServer();
  vtkSMSession * session = server->session();

  vtkNew<vtkSMProxyIterator> iter;
  iter->SetSession(session);
  iter->SetModeToOneGroup();
  for (iter->Begin("settings"); !iter->IsAtEnd(); iter->Next())
    {
    vtkSMProxy* proxy = iter->GetProxy();
    if (proxy)
      {
      proxy->ResetPropertiesToXMLDefaults();
      }
    }

  vtkSMProxy* paletteProxy = server->proxyManager()->GetProxy("global_properties", "ColorPalette");
  if (paletteProxy)
    {
    paletteProxy->ResetPropertiesToXMLDefaults();
    }
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::onTabIndexChanged(int index)
{
  int stackWidgetIndex = this->Internals->TabToStackedWidgets[index];
  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
  ui.stackedWidget->setCurrentIndex(stackWidgetIndex);
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::filterPanelWidgets()
{
  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
  emit this->filterWidgets(
    ui.SearchBox->isAdvancedSearchActive(), ui.SearchBox->text());
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::onChangeAvailable()
{
//  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
//  ui.buttonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
//  ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::showRestartRequiredMessage()
{
  Ui::CustomSettingsWidget &ui = this->Internals->Ui;
  ui.restartRequiredLabel->setVisible(true);
  PVGUI_ParaViewSettingsPane::ShowRestartRequired = true;
}

//-----------------------------------------------------------------------------
void PVGUI_ParaViewSettingsPane::saveInQSettings(
  const char* key, vtkSMProperty* smproperty)
{
  pqSettings* settings = pqApplicationCore::instance()->settings();

  // FIXME: handle all property types. This will only work for single value
  // properties.
  if (smproperty->IsA("vtkSMIntVectorProperty") ||
    smproperty->IsA("vtkSMIdTypeVectorProperty"))
    {
    settings->setValue(key, vtkSMPropertyHelper(smproperty).GetAsInt());
    }
  else if (smproperty->IsA("vtkSMDoubleVectorProperty"))
    {
    settings->setValue(key, vtkSMPropertyHelper(smproperty).GetAsDouble());
    }
  else if (smproperty->IsA("vtkSMStringVectorProperty"))
    {
    settings->setValue(key, vtkSMPropertyHelper(smproperty).GetAsString());
    }
}

