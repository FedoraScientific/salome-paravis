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

#include "pqCustomXYChartDisplayPanel.h"
#include "ui_CustomXYChartDisplayPanel.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkSMChartRepresentationProxy.h"
#include "vtkSMPropertyHelper.h"
#include "vtkDataArray.h"
#include "vtkDataObject.h"
#include "vtkSMArraySelectionDomain.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSmartPointer.h"
#include "vtkTable.h"
#include "vtkChart.h"
#include "vtkWeakPointer.h"

#include <QColorDialog>
#include <QHeaderView>
#include <QList>
#include <QPointer>
#include <QPixmap>
#include <QSortFilterProxyModel>
#include <QDebug>

#include "pqDataInformationModel.h"
#include "pqComboBoxDomain.h"
#include "pqPropertyLinks.h"
#include "pqSignalAdaptorCompositeTreeWidget.h"
#include "pqSignalAdaptors.h"
#include "pqSMAdaptor.h"
#include "pqXYChartView.h"
#include "pqDataRepresentation.h"
#include "pqCustomPlotSettingsModel.h"

#include <assert.h>

//-----------------------------------------------------------------------------
class pqCustomXYChartDisplayPanel::pqInternal : public Ui::CustomXYChartDisplayPanel
{
public:
  pqInternal()
    {
    this->SettingsModel = 0;
    this->XAxisArrayDomain = 0;
    this->XAxisArrayAdaptor = 0;

    this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    }

  ~pqInternal()
    {
    delete this->SettingsModel;
    delete this->XAxisArrayDomain;
    delete this->XAxisArrayAdaptor;
    }

  vtkWeakPointer<vtkSMChartRepresentationProxy> ChartRepresentation;
  pqCustomPlotSettingsModel* SettingsModel;
  pqComboBoxDomain* XAxisArrayDomain;
  pqSignalAdaptorComboBox* XAxisArrayAdaptor;
  pqPropertyLinks Links;

  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;

  bool InChange;
};

//-----------------------------------------------------------------------------
pqCustomXYChartDisplayPanel::pqCustomXYChartDisplayPanel(
  pqRepresentation* display,QWidget* p)
: pqDisplayPanel(display, p)
{
  this->Internal = new pqCustomXYChartDisplayPanel::pqInternal();
  this->Internal->setupUi(this);

  this->Internal->SettingsModel = new pqCustomPlotSettingsModel(this);
  this->Internal->SeriesList->setModel(this->Internal->SettingsModel);

  this->Internal->XAxisArrayAdaptor = new pqSignalAdaptorComboBox(
    this->Internal->XAxisArray);

  QObject::connect(
    this->Internal->SeriesList, SIGNAL(activated(const QModelIndex &)),
    this, SLOT(activateItem(const QModelIndex &)));
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  QObject::connect(model,
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    this, SLOT(updateOptionsWidgets()));
  QObject::connect(model,
    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(updateOptionsWidgets()));
  QObject::connect(this->Internal->SettingsModel, SIGNAL(modelReset()),
    this, SLOT(updateOptionsWidgets()));
  QObject::connect(this->Internal->SettingsModel, SIGNAL(redrawChart()),
    this, SLOT(updateAllViews()));
  QObject::connect(this->Internal->XAxisArray, SIGNAL(currentIndexChanged(int)),
    this, SLOT(updateAllViews()));

  QObject::connect(this->Internal->UseArrayIndex, SIGNAL(toggled(bool)),
    this, SLOT(useArrayIndexToggled(bool)));
  QObject::connect(this->Internal->UseDataArray, SIGNAL(toggled(bool)),
    this, SLOT(useDataArrayToggled(bool)));

  QObject::connect(
    this->Internal->ColorButton, SIGNAL(chosenColorChanged(const QColor &)),
    this, SLOT(setCurrentSeriesColor(const QColor &)));
  QObject::connect(this->Internal->Thickness, SIGNAL(valueChanged(int)),
    this, SLOT(setCurrentSeriesThickness(int)));
  QObject::connect(this->Internal->StyleList, SIGNAL(currentIndexChanged(int)),
    this, SLOT(setCurrentSeriesStyle(int)));
  QObject::connect(this->Internal->AxisList, SIGNAL(currentIndexChanged(int)),
    this, SLOT(setCurrentSeriesAxes(int)));
  QObject::connect(this->Internal->MarkerStyleList, SIGNAL(currentIndexChanged(int)),
    this, SLOT(setCurrentSeriesMarkerStyle(int)));

  QObject::connect(
    this->Internal->AutoSelect, SIGNAL(toggled(bool)),
    this, SLOT(autoSelectToggled(bool)));

  QObject::connect(
    this->Internal->IgnoreUnits, SIGNAL(toggled(bool)),
    this, SLOT(ignoreUnitsToggled(bool)));

  QObject::connect(
    this->Internal->GenerateAxesTitles, SIGNAL(toggled(bool)),
    this, SLOT(updateViewOptions()));
  QObject::connect(this->Internal->SettingsModel, SIGNAL(redrawChart()),
    this, SLOT(updateViewOptions()));

  resetUnitsControls();

  this->setDisplay(display);

  QObject::connect(&this->Internal->Links, SIGNAL(qtWidgetChanged()),
                   this, SLOT(reloadSeries()), Qt::QueuedConnection);
  QObject::connect(&this->Internal->Links, SIGNAL(qtWidgetChanged()),
                   this->Internal->SettingsModel, SLOT(reload()));
}

//-----------------------------------------------------------------------------
pqCustomXYChartDisplayPanel::~pqCustomXYChartDisplayPanel()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::reloadSeries()
{
  updateViewOptions();
  
  this->updateAllViews();
  this->updateOptionsWidgets();
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setDisplay(pqRepresentation* disp)
{
  this->setEnabled(false);

  vtkSMChartRepresentationProxy* proxy =
    vtkSMChartRepresentationProxy::SafeDownCast(disp->getProxy());
  this->Internal->ChartRepresentation = proxy;
  if (!this->Internal->ChartRepresentation)
    {
    qWarning() << "pqCustomXYChartDisplayPanel given a representation proxy "
                  "that is not an XYChartRepresentation. Cannot edit.";
    return;
    }

  // this is essential to ensure that when you undo-redo, the representation is
  // indeed update-to-date, thus ensuring correct domains etc.
  proxy->UpdatePipeline();

  // The model for the plot settings
  this->Internal->SettingsModel->setRepresentation(
      qobject_cast<pqDataRepresentation*>(disp));

  // Connect to the new properties.pqComboBoxDomain will ensure that
  // when ever the domain changes the widget is updated as well.
  this->Internal->XAxisArrayDomain = new pqComboBoxDomain(
      this->Internal->XAxisArray, proxy->GetProperty("XArrayName"));
  this->Internal->Links.addPropertyLink(this->Internal->XAxisArrayAdaptor,
      "currentText", SIGNAL(currentTextChanged(const QString&)),
      proxy, proxy->GetProperty("XArrayName"));

  // Link to set whether the index is used for the x axis
  this->Internal->Links.addPropertyLink(
    this->Internal->UseArrayIndex, "checked",
    SIGNAL(toggled(bool)),
    proxy, proxy->GetProperty("UseIndexForXAxis"));

  // Proxy changed
  this->Internal->VTKConnect->Connect(proxy->GetProperty("SeriesNamesInfo"),
	vtkCommand::PropertyModifiedEvent, this, SLOT(resetUnitsControls()));

  this->changeDialog(disp);

  this->setEnabled(true);

  this->reloadSeries();
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::changeDialog(pqRepresentation* disp)
{
  vtkSMChartRepresentationProxy* proxy =
    vtkSMChartRepresentationProxy::SafeDownCast(disp->getProxy());
  bool visible = true;
  if (QString("Bar") == vtkSMPropertyHelper(proxy,"ChartType").GetAsString())
    {
      visible = false;
    }
  
  this->Internal->Thickness->setVisible(visible);
  this->Internal->ThicknessLabel->setVisible(visible);
  this->Internal->StyleList->setVisible(visible);
  this->Internal->StyleListLabel->setVisible(visible);
  this->Internal->MarkerStyleList->setVisible(visible);
  this->Internal->MarkerStyleListLabel->setVisible(visible);
  this->Internal->AxisList->setVisible(false);
  this->Internal->AxisListLabel->setVisible(false);
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::activateItem(const QModelIndex &index)
{
  if(!this->Internal->ChartRepresentation
      || !index.isValid() || index.column() != 1)
    {
    // We are interested in clicks on the color swab alone.
    return;
    }

  // Get current color
  QColor color = this->Internal->SettingsModel->getSeriesColor(index.row());

  // Show color selector dialog to get a new color
  color = QColorDialog::getColor(color, this);
  if (color.isValid())
    {
    // Set the new color
    this->Internal->SettingsModel->setSeriesColor(index.row(), color);
    this->Internal->ColorButton->blockSignals(true);
    this->Internal->ColorButton->setChosenColor(color);
    this->Internal->ColorButton->blockSignals(false);
    this->updateAllViews();
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::updateOptionsWidgets()
{
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  if(model)
    {
    // Show the options for the current item.
    QModelIndex current = model->currentIndex();
    QModelIndexList indexes = model->selectedIndexes();
    if((!current.isValid() || !model->isSelected(current)) &&
        indexes.size() > 0)
      {
      current = indexes.last();
      }

    this->Internal->ColorButton->blockSignals(true);
    this->Internal->Thickness->blockSignals(true);
    this->Internal->StyleList->blockSignals(true);
    this->Internal->MarkerStyleList->blockSignals(true);
    this->Internal->AxisList->blockSignals(true);
    if (current.isValid())
      {
      int seriesIndex = current.row();
      QColor color = this->Internal->SettingsModel->getSeriesColor(seriesIndex);
      this->Internal->ColorButton->setChosenColor(color);
      this->Internal->Thickness->setValue(
        this->Internal->SettingsModel->getSeriesThickness(seriesIndex));
      this->Internal->StyleList->setCurrentIndex(
        this->Internal->SettingsModel->getSeriesStyle(seriesIndex));
      this->Internal->MarkerStyleList->setCurrentIndex(
        this->Internal->SettingsModel->getSeriesMarkerStyle(seriesIndex));
      this->Internal->AxisList->setCurrentIndex(
        this->Internal->SettingsModel->getSeriesAxisCorner(seriesIndex));
      }
    else
      {
      this->Internal->ColorButton->setChosenColor(Qt::white);
      this->Internal->Thickness->setValue(1);
      this->Internal->StyleList->setCurrentIndex(0);
      this->Internal->MarkerStyleList->setCurrentIndex(0);
      this->Internal->AxisList->setCurrentIndex(0);
      }

    this->Internal->ColorButton->blockSignals(false);
    this->Internal->Thickness->blockSignals(false);
    this->Internal->StyleList->blockSignals(false);
    this->Internal->MarkerStyleList->blockSignals(false);
    this->Internal->AxisList->blockSignals(false);

    // Disable the widgets if nothing is selected or current.
    bool hasItems = indexes.size() > 0;
    this->Internal->ColorButton->setEnabled(hasItems);
    this->Internal->Thickness->setEnabled(hasItems);
    this->Internal->StyleList->setEnabled(hasItems);
    this->Internal->MarkerStyleList->setEnabled(hasItems);
    this->Internal->AxisList->setEnabled(hasItems);
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setCurrentSeriesColor(const QColor &color)
{
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  if(model)
    {
    this->Internal->InChange = true;
    QModelIndexList indexes = model->selectedIndexes();
    QModelIndexList::Iterator iter = indexes.begin();
    for( ; iter != indexes.end(); ++iter)
      {
      this->Internal->SettingsModel->setSeriesColor(iter->row(), color);
      }
    this->Internal->InChange = false;
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setCurrentSeriesThickness(int thickness)
{
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  if (model)
    {
    this->Internal->InChange = true;
    QModelIndexList indexes = model->selectedIndexes();
    QModelIndexList::Iterator iter = indexes.begin();
    for( ; iter != indexes.end(); ++iter)
      {
      this->Internal->SettingsModel->setSeriesThickness(iter->row(), thickness);
      }
    this->Internal->InChange = false;
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setCurrentSeriesStyle(int style)
{
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  if (model)
    {
    this->Internal->InChange = true;
    QModelIndexList indexes = model->selectedIndexes();
    QModelIndexList::Iterator iter = indexes.begin();
    for( ; iter != indexes.end(); ++iter)
      {
      this->Internal->SettingsModel->setSeriesStyle(iter->row(), style);
      }
    this->Internal->InChange = false;
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setCurrentSeriesMarkerStyle(int style)
{
  QItemSelectionModel *model = this->Internal->SeriesList->selectionModel();
  if (model)
    {
    this->Internal->InChange = true;
    QModelIndexList indexes = model->selectedIndexes();
    QModelIndexList::Iterator iter = indexes.begin();
    for( ; iter != indexes.end(); ++iter)
      {
      this->Internal->SettingsModel->setSeriesMarkerStyle(iter->row(), style);
      }
    this->Internal->InChange = false;
    }
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::setCurrentSeriesAxes(int)
{

}

//-----------------------------------------------------------------------------
Qt::CheckState pqCustomXYChartDisplayPanel::getEnabledState() const
{
  Qt::CheckState enabledState = Qt::Unchecked;

  return enabledState;
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::useArrayIndexToggled(bool toggle)
{
  this->Internal->UseDataArray->setChecked(!toggle);
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::useDataArrayToggled(bool toggle)
{
  this->Internal->UseArrayIndex->setChecked(!toggle);
  this->updateAllViews();
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::autoSelectToggled(bool checked)
{
  this->Internal->SettingsModel->SetAutoSelectModeOn(checked);
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::ignoreUnitsToggled(bool checked)
{
  this->Internal->AutoSelect->setEnabled(!checked);
  this->Internal->SettingsModel->SetIgnoreUnitsModeOn(checked);
  this->Internal->SettingsModel->SetAutoSelectModeOn(!checked && 
						     this->Internal->AutoSelect->isChecked());
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::resetUnitsControls()
{
  this->Internal->IgnoreUnits->setCheckState(Qt::Checked);
  this->Internal->AutoSelect->setCheckState(Qt::Unchecked);
}

//-----------------------------------------------------------------------------
void pqCustomXYChartDisplayPanel::updateViewOptions()
{
  pqRepresentation* disp = this->getRepresentation();
  if (!disp || !this->Internal->ChartRepresentation)
    {
      return;
    }

  pqXYChartView* view = qobject_cast<pqXYChartView*>(disp->getView());
  if (view && view->getProxy())
    {
      vtkSMProxy* proxy = view->getProxy();
      
      QList<QVariant> values = 
	pqSMAdaptor::getMultipleElementProperty(proxy->GetProperty("AxisTitle"));
      if (values.size() < 2)
	{
	  return;
	}
      
      if (!this->Internal->GenerateAxesTitles->isChecked())
	{
	  values.insert(0, QVariant(""));
	  values.insert(1, QVariant(""));
	}
      else
	{
	  // Set X axis title
	  int useIndexForXAxis = vtkSMPropertyHelper(
              this->Internal->ChartRepresentation, "UseIndexForXAxis").GetAsInt();
	  
	  QString xTitle("");
	  if (!useIndexForXAxis) 
	    {
	      xTitle = vtkSMPropertyHelper(this->Internal->ChartRepresentation, "XArrayName")
		.GetAsString();
	    }
      
	  // Set Y axis title
	  int nbRows = this->Internal->SettingsModel->rowCount(QModelIndex());
	  int nbEnabled = 0;
	  int row = 0;
	  for (int i = 0; i < nbRows && nbEnabled < 2; i++)
	    {
	      if (this->Internal->SettingsModel->getSeriesEnabled(i))
		{
		  nbEnabled++;
		  row = i;
		}
	    }
	  
	  QString yTitle("");
	  if (nbEnabled == 1)
	    {
	      yTitle = this->Internal->SettingsModel->getSeriesName(row);
	    }
	  values.insert(0, QVariant(yTitle));
	  values.insert(1, QVariant(xTitle));
	}

      pqSMAdaptor::setMultipleElementProperty(proxy->GetProperty("AxisTitle"), values);

      view->getProxy()->UpdateVTKObjects();
      view->render();
    }
}
