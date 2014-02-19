// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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
// Author : Anthony Geay

#include "VectBoolSpreadSheet.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTimeEdit>
#include <QPainter>
#include <iostream>

VectBoolModel::VectBoolModel(int maxSize, int nbRows):_activated(maxSize,false),_nb_rows(nbRows)
{
  setCurSize(maxSize);
}

int VectBoolModel::rowCount(const QModelIndex &) const
{
  return _nb_rows;
}

int VectBoolModel::columnCount (const QModelIndex &) const
{
  int sz(curSize());
  if(sz%_nb_rows==0)
    return sz/_nb_rows;
  else
    return sz/_nb_rows+1;
}

QVariant VectBoolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role==Qt::FontRole)
    {
      QFont serifFont("Arial",6, QFont::Bold);
      return QVariant(serifFont);
    }
  else if(role==Qt::DisplayRole)
    {
      return QVariant(section);
    }
  else
    return QAbstractTableModel::headerData(section,orientation,role);
}

bool VectBoolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(role==Qt::UserRole)
    {
      int pos(index.column()*_nb_rows+index.row());
      bool v(_activated[pos]);
      _activated[pos]=!v;
      emit nbOfTimeStepsOnChanged((int)getNbOfActivatedTimeSteps(),_dts.size());
      return true;
    }
  else
    return QAbstractTableModel::setData(index,value,role);
}

QVariant VectBoolModel::data(const QModelIndex &index, int role) const
{
  if(role==Qt::FontRole)
    {
      QFont serifFont("Arial",8);//, QFont::Bold);
      return QVariant(serifFont);
    }
  else if(role==Qt::UserRole)
    {
      int pos(index.column()*_nb_rows+index.row());
      return QVariant(_activated[pos]);
    }
  else if(role==Qt::DisplayRole)
    {
      int pos(index.column()*_nb_rows+index.row());
      if(pos<curSize())
        return QVariant(QString::number(pos));
      else
        return QVariant();
    }
  else if(role==Qt::TextAlignmentRole)
    {
      return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
    }
  else if(role==Qt::ToolTipRole)
    {
      QVariant v(data(index,Qt::DisplayRole));
      QString v2(v.toString());
      int pos(v2.toInt());
      QString v3(_activated[pos]?QString("ON"):QString("OFF"));
      QString v4(QString("time #%1 (it=%2 order=%3 t=%4) (%5)").arg(v2).arg(_dts[pos]).arg(_its[pos]).arg(_tts[pos]).arg(v3));
      return QVariant(v4);
    }
  else if(role==Qt::BackgroundRole)
    {
      int pos(index.column()*_nb_rows+index.row());
      if(pos>=curSize())
        return QVariant();
      if(_activated[pos])
        {
          QBrush b(QColor(0,255,0));
          return QVariant(b);
        }
      else
        {
          QBrush b(QColor(255,0,0));
          return QVariant(b);
        }
    }
  else
    return QVariant();
}

bool VectBoolModel::setCurrentItems(const QStringList& dts, const QStringList& its, const QStringList& tts)
{
  int oldSize(curSize());
  if(oldSize!=dts.size())
    {
      emit layoutAboutToBeChanged();
      _dts=dts; _its=its; _tts=tts;
      emit layoutChanged();
      return true;
    }
  else
    {
      _dts=dts; _its=its; _tts=tts;
      return false; 
    }
  
}

void VectBoolModel::setNumberOfRows(int newNbOfRows)
{
  if(newNbOfRows!=_nb_rows)
    {
      emit beginResetModel();
      _nb_rows=newNbOfRows;
      emit endResetModel();
    }
}

void VectBoolModel::selectUnselectAll()
{
  int nbOn(getNbOfActivatedTimeSteps()),sz(curSize()),signalVal(0);
  emit layoutAboutToBeChanged();
  if(nbOn>sz/2)
    {
      for(std::size_t ii=0;ii<sz;ii++)
        _activated[ii]=false;
      signalVal=0;
    }
  else
    {
      for(std::size_t ii=0;ii<sz;ii++)
        _activated[ii]=true;
      signalVal=(int)sz;
    }
  emit layoutChanged();
  emit nbOfTimeStepsOnChanged(signalVal,_dts.size());
}

int VectBoolModel::getNbOfActivatedTimeSteps() const
{
  int sz(curSize()),nbOn(0);
  for(std::size_t ii=0;ii<sz;ii++)
    if(_activated[ii])
      nbOn++;
  return nbOn;
}

void VectBoolModel::setCurSize(int sz)
{
  _dts.clear(); _its.clear(); _tts.clear();
  for(int i=0;i<sz;i++)
    {
      _dts.push_back(QString());
      _its.push_back(QString());
      _tts.push_back(QString());
    }
}

int VectBoolModel::curSize() const
{
  return _dts.size();
}

///////////////

VectBoolSpreadSheet::VectBoolSpreadSheet(QWidget *parent):QTableView(parent),_delegate(new OnOffDelegate)
{
}

VectBoolSpreadSheet::~VectBoolSpreadSheet()
{
  delete _delegate;
}

void VectBoolSpreadSheet::init()
{
  this->horizontalHeader()->setMinimumSectionSize(2);
  this->horizontalHeader()->setDefaultSectionSize(2);
  this->verticalHeader()->setMinimumSectionSize(2);
  this->verticalHeader()->setDefaultSectionSize(2);
  this->setItemDelegate(_delegate);
  this->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
  //this->verticalHeader()->hide();
  //this->horizontalHeader()->hide();
}

void VectBoolSpreadSheet::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
  QAbstractItemModel * m(model());
  foreach(const QModelIndex& ind,selected.indexes())
    {
      m->setData(ind,QVariant(true),Qt::UserRole);
    }
  QTableView::selectionChanged(selected,deselected);
}

void VectBoolSpreadSheet::nbOfRowsHasChanged(int newNbOfRows)
{
  VectBoolModel *zeModel(qobject_cast<VectBoolModel *>(model()));
  if(!zeModel)
    return ;
  zeModel->setNumberOfRows(newNbOfRows);
  this->verticalHeader()->setUpdatesEnabled(true);//please let this line. If not a refresh problem appear at EDF configuration.
}

void VectBoolSpreadSheet::selectUnselectAllFired()
{
  VectBoolModel *zeModel(qobject_cast<VectBoolModel *>(model()));
  if(!zeModel)
    return ;
  zeModel->selectUnselectAll();
}

///////////////

OnOffDelegate::OnOffDelegate(QObject *parent):QStyledItemDelegate(parent)
{
}

void OnOffDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex& index) const
{
  painter->save();
  const QAbstractItemModel *zeModel(index.model());
  QString cont(zeModel->data(index,Qt::DisplayRole).toString());
  bool checked(zeModel->data(index,Qt::UserRole).toBool());
  QFont font;
  if(checked)
    {
      QFont zeFont(zeModel->data(index,Qt::FontRole).value<QFont>());
      zeFont.setBold(true);
      font=zeFont;
    }
  else
    {
      font=QFont("Arial",7);
      font.setItalic(true);
    }
  painter->setFont(font);
  Qt::Alignment al((Qt::Alignment)zeModel->data(index,Qt::TextAlignmentRole).toInt());
  if(checked)
    {//sizeHint
      //painter->drawEllipse(option.rect);
      //painter->setBrush(QBrush(Qt::lightGray,Qt::Dense6Pattern));
      painter->setBrush(QBrush(QColor(230,230,255)));
      painter->drawRect(option.rect);
      //painter->drawLine(option.rect.topLeft(),option.rect.bottomRight());
      //painter->drawLine(option.rect.topRight(),option.rect.bottomLeft());
    }
  else
    {
      painter->setBrush(QBrush(QColor(255,255,255)));
      painter->drawRect(option.rect);
      painter->setPen(Qt::lightGray);
    }
  painter->drawText(option.rect,cont,QTextOption(al));
  painter->restore();
}
