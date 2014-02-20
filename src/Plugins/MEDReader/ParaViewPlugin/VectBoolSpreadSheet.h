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

#include <QTableWidget>
#include <QItemDelegate>
#include <QStringListModel>
#include <QAbstractTableModel>
#include <QListView>
#include <QStyledItemDelegate>

class OnOffDelegate : public QStyledItemDelegate//QItemDelegate
{
  Q_OBJECT
  public:
  OnOffDelegate(QObject *parent=0);
  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

class VectBoolSpreadSheet : public QTableView
{
  Q_OBJECT
public:
  VectBoolSpreadSheet(QWidget *parent);
  ~VectBoolSpreadSheet();
  void init();
  void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
public slots:
  void nbOfRowsHasChanged(int newNbOfRows);
  void selectUnselectAllFired();
private:
  OnOffDelegate *_delegate;
};

class VectBoolModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  VectBoolModel(int maxSize, int nbRows);
  std::size_t getSize() const { return _activated.size(); }
  bool getStatusAt(int pos) const { return _activated[pos]; }
  void setStatusAt(int pos, bool val) { _activated[pos]=val; }
  int rowCount( const QModelIndex & parent = QModelIndex() ) const;
  int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  // non const methods.
  bool setCurrentItems(const QStringList& dts, const QStringList& its, const QStringList& tts);
  int getNbOfActivatedTimeSteps() const;
public:
  void setNumberOfRows(int newNbOfRows);
  void selectUnselectAll();
signals:
  void nbOfTimeStepsOnChanged(int newNbOfTimeStepsOn, int totalNbOfTS);
private:
  void setCurSize(int sz);
  int curSize() const;
private:
  std::vector<bool> _activated;
  QStringList _dts,_its,_tts;
  int _cur_size;
  int _nb_rows;
};
