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

#ifndef __pqCustomPlotSettingsModel_h
#define __pqCustomPlotSettingsModel_h

#include "pqPlotSettingsModel.h"

class pqDataRepresentation;

class pqCustomPlotSettingsModel : public pqPlotSettingsModel
{
  typedef pqPlotSettingsModel Superclass;

  Q_OBJECT

public:
  pqCustomPlotSettingsModel(QObject* parent = 0);
  ~pqCustomPlotSettingsModel();

  void setRepresentation(pqDataRepresentation* rep);
  pqDataRepresentation* representation() const;

  /// \brief Sets the data for the given model index.
  /// \param index The model index.
  /// \param value The new data for the given role.
  /// \param role The role to set data for.
  /// \return True if the data was changed successfully.
  virtual bool setData(const QModelIndex &index, const QVariant &value,
                       int role=Qt::EditRole);

  void SetIgnoreUnitsModeOn(bool enabled);
  void SetAutoSelectModeOn(bool enabled);

public slots:

  // Description:
  // API to enable series.
  void setSeriesEnabled(int row, bool enabled);

private:
  QString getUnit(const QString& seriesName); 
  
private:
  class pqImplementation;
  pqImplementation* Implementation;

  bool IgnoreUnitsModeOn;
  bool AutoSelectModeOn;
};

#endif
