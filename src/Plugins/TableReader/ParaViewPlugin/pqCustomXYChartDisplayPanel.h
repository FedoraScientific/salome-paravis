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

#ifndef __pqCustomXYChartDisplayPanel_h
#define __pqCustomXYChartDisplayPanel_h

#include "pqDisplayPanel.h"

class pqRepresentation;
class pqDataInformationModel;
class QModelIndex;

/// Editor widget for XY chart displays.
class pqCustomXYChartDisplayPanel : public pqDisplayPanel
{
  Q_OBJECT
public:
  pqCustomXYChartDisplayPanel(pqRepresentation* display, QWidget* parent=0);
  virtual ~pqCustomXYChartDisplayPanel();

public slots:
  /// Reloads the series list from the display.
  void reloadSeries();

protected slots:
  /// Slot to listen to clicks for changing color.
  void activateItem(const QModelIndex &index);

  void updateOptionsWidgets();

  void setCurrentSeriesColor(const QColor &color);

  void setCurrentSeriesThickness(int thickness);

  void setCurrentSeriesStyle(int listIndex);

  void setCurrentSeriesAxes(int listIndex);

  void setCurrentSeriesMarkerStyle(int listIndex);

  void useArrayIndexToggled(bool);

  void useDataArrayToggled(bool);

  /// Unit controls
  void autoSelectToggled(bool checked);
  void ignoreUnitsToggled(bool checked);

  /// Reset state of the controls relative to units
  void resetUnitsControls();

  /// Update view options
  void updateViewOptions();

private:
  pqCustomXYChartDisplayPanel(const pqCustomXYChartDisplayPanel&); // Not implemented.
  void operator=(const pqCustomXYChartDisplayPanel&); // Not implemented.

  /// Set the display whose properties this editor is editing.
  /// This call will raise an error if the display is not
  /// an XYChartRepresentation proxy.
  void setDisplay(pqRepresentation* display);

  /// Disable/enable elements of the dialog based on the chart type.
  void changeDialog(pqRepresentation* display);
  
  Qt::CheckState getEnabledState() const;

  class pqInternal;
  pqInternal* Internal;

  pqDataInformationModel* Model;
};

#endif
