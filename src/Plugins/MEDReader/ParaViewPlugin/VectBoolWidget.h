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

#ifndef __VECTBOOLWIDGET_H__
#define __VECTBOOLWIDGET_H__

#include <QFrame>
#include <QLabel>

class QSlider;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPushButton;
class VectBoolModel;
class VectBoolSpreadSheet;

class NbOfActiveTSDisplay : public QLabel
{
Q_OBJECT
public:
  NbOfActiveTSDisplay(QWidget *parent=0);
public slots:
  void display(int nbOfItemsOn, int totalNbOfItems);
  void totalNbOfItemsChanged(int totalNbOfItems);
private:
  int _nb_of_items_on;
  int _total_nb_of_items;
};

class VectBoolItem : public QObject
{
Q_OBJECT
Q_PROPERTY(bool activated READ isActivated WRITE activated)
public:
  VectBoolItem(QObject *parent, int pos, VectBoolModel *model);
  bool isActivated() const;
  void activated(bool val);
public slots:
  void theModelHasChanged();
signals:
  void changed();
private:
  int _pos;
  mutable bool _old_status;
  VectBoolModel *_model;
};

class VectBoolWidget : public QFrame
{
public:
  VectBoolWidget(QWidget *parent, int nbOfItems);
  ~VectBoolWidget();
  int getNumberOfBoolItems() const;
  void setItems(const QStringList& dts, const QStringList& its, const QStringList& tts);
  const QVector<VectBoolItem *>& getItems() const { return _items; }
private:
  static int GetMaxTickValueOfSlider(int val);
private:
  QVector<VectBoolItem *> _items;
  QHBoxLayout *_horizontal_layout;
  QFrame *_spreadsheet_container;
  QGridLayout *_grid_layout;
  QGroupBox *_group_box;
  QVBoxLayout *_vertical_layout;
  NbOfActiveTSDisplay *_nb_of_time_steps_on;
  QSlider *_nb_of_rows_selecter;
  QPushButton *_sel_unsel_all;
  VectBoolSpreadSheet *_vbw;
  VectBoolModel *_vbm;
};

#endif
