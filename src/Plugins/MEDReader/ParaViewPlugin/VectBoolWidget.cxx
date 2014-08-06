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

#include "VectBoolWidget.h"
#include "VectBoolSpreadSheet.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QSlider>

#include <cmath>
#include <iostream>

NbOfActiveTSDisplay::NbOfActiveTSDisplay(QWidget *parent):QLabel(parent),_nb_of_items_on(0),_total_nb_of_items(0)
{
  setFont(QFont("Arial",8,QFont::Bold));
  setTextFormat(Qt::AutoText);
}

  void NbOfActiveTSDisplay::display(int nbOfItemsOn, int totalNbOfItems)
{
  _nb_of_items_on=nbOfItemsOn;
  _total_nb_of_items=totalNbOfItems;
  setText(QString("%1/%2").arg(_nb_of_items_on).arg(_total_nb_of_items));
}

void NbOfActiveTSDisplay::totalNbOfItemsChanged(int totalNbOfItems)
{
  _total_nb_of_items=totalNbOfItems;
  setText(QString("%1/%2").arg(_nb_of_items_on).arg(_total_nb_of_items));
}

VectBoolItem::VectBoolItem(QObject *parent, int pos, VectBoolModel *model):QObject(parent),_pos(pos),_old_status(true),_model(model)
{
}

bool VectBoolItem::isActivated() const
{
  return _model->getStatusAt(_pos);
}

void VectBoolItem::activated(bool val)
{
  //_model->setStatusAt(_pos,val);
}

void VectBoolItem::theModelHasChanged()
{
  bool val(isActivated());
  if(val!=_old_status)
    {
      _old_status=val;
      emit changed();
    }
}

VectBoolWidget::VectBoolWidget(QWidget *parent, int nbOfItems):QFrame(parent)
{
  QSizePolicy sizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);
  this->setFrameShape(QFrame::StyledPanel);
  this->setFrameShadow(QFrame::Raised);
  _horizontal_layout=new QHBoxLayout(this);
  _horizontal_layout->setSpacing(6);
  _horizontal_layout->setContentsMargins(11,11,11,11);
  _spreadsheet_container=new QFrame(this);
  QSizePolicy sizePolicy1(QSizePolicy::Expanding,QSizePolicy::Expanding);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(_spreadsheet_container->sizePolicy().hasHeightForWidth());
  _spreadsheet_container->setSizePolicy(sizePolicy1);
  _grid_layout=new QGridLayout(_spreadsheet_container);
  _grid_layout->setSpacing(6);
  _grid_layout->setContentsMargins(11,11,11,11);
  _horizontal_layout->addWidget(_spreadsheet_container);
  _group_box=new QGroupBox(this);
  QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(_group_box->sizePolicy().hasHeightForWidth());
  _group_box->setSizePolicy(sizePolicy2);
  //
  _vertical_layout=new QVBoxLayout(_group_box);
  _vertical_layout->setSpacing(6);
  _vertical_layout->setContentsMargins(11,11,11,11);
  _nb_of_time_steps_on=new NbOfActiveTSDisplay(_group_box);
  _nb_of_time_steps_on->setMaximumSize(QSize(40,16777215));
  _vertical_layout->addWidget(_nb_of_time_steps_on);
  _nb_of_rows_selecter=new QSlider(_group_box);
  QSizePolicy sizePolicy3(QSizePolicy::Expanding,QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(_nb_of_rows_selecter->sizePolicy().hasHeightForWidth());
  _nb_of_rows_selecter->setSizePolicy(sizePolicy3);
  _nb_of_rows_selecter->setOrientation(Qt::Vertical);
  _vertical_layout->addWidget(_nb_of_rows_selecter);
  _sel_unsel_all=new QPushButton(_group_box);
  _sel_unsel_all->setMaximumSize(QSize(40,16777215));
  _sel_unsel_all->setText(QString("Sel All"));
  QFont font("Andale Mono");
  font.setPointSize(5);
  _sel_unsel_all->setFont(font);
  _vertical_layout->addWidget(_sel_unsel_all);
  //
  _horizontal_layout->addWidget(_group_box);
  //
  _vbw=new VectBoolSpreadSheet(_spreadsheet_container);
  QSizePolicy sizePolicy4(QSizePolicy::Expanding,QSizePolicy::Expanding);
  sizePolicy4.setHorizontalStretch(0);
  sizePolicy4.setVerticalStretch(0);
  sizePolicy4.setHeightForWidth(_vbw->sizePolicy().hasHeightForWidth());
  _vbw->setSizePolicy(sizePolicy4);
  _nb_of_rows_selecter->setSizePolicy(sizePolicy3);
  _nb_of_rows_selecter->setMinimum(1);
  _nb_of_rows_selecter->setMaximum(GetMaxTickValueOfSlider(nbOfItems));
  _nb_of_rows_selecter->setTickInterval(1);
  _nb_of_rows_selecter->setTickPosition(QSlider::TicksRight);
  connect(_nb_of_rows_selecter,SIGNAL(valueChanged(int)),_vbw,SLOT(nbOfRowsHasChanged(int)));
  connect(_sel_unsel_all,SIGNAL(clicked()),_vbw,SLOT(selectUnselectAllFired()));
  _grid_layout->addWidget(_vbw);
  _vbw->init();
  _vbm=new VectBoolModel(nbOfItems,1);
  _vbw->setModel(_vbm);
  connect(this->_vbm,SIGNAL(nbOfTimeStepsOnChanged(int,int)),_nb_of_time_steps_on,SLOT(display(int,int)));
  _vbm->selectUnselectAll();
  _nb_of_rows_selecter->setValue(_nb_of_rows_selecter->maximum());
  //
  _items.resize(nbOfItems);
  for(int i=0;i<nbOfItems;i++)
    {
      _items[i]=new VectBoolItem(this,i,_vbm);
      connect(_vbm,SIGNAL(nbOfTimeStepsOnChanged(int,int)),_items[i],SLOT(theModelHasChanged()));
    }
}

void VectBoolWidget::setItems(const QStringList& dts, const QStringList& its, const QStringList& tts)
{
  int curNbOfItems(dts.size());
  if(curNbOfItems>getNumberOfBoolItems())
    {
      std::cerr << "ERROR ! Internal Error in VectBoolWidget::setItems ! Current number of items is bigger than the current one !" << std::endl;
      return ;
    }
  if(_vbm->setCurrentItems(dts,its,tts))
    {
      _nb_of_rows_selecter->setMaximum(GetMaxTickValueOfSlider(curNbOfItems));
      _nb_of_rows_selecter->setValue(_nb_of_rows_selecter->maximum());
      _nb_of_time_steps_on->display(_vbm->getNbOfActivatedTimeSteps(),dts.size());
    }
}

int VectBoolWidget::getNumberOfBoolItems() const
{
  return (int)_vbm->getSize();
}

int VectBoolWidget::GetMaxTickValueOfSlider(int val)
{
  double valf((double)val);
  double ret(sqrt(valf));
  int reti((int)ret);
  if(reti*reti==val)
    return reti;
  else
    return reti+1;
}

VectBoolWidget::~VectBoolWidget()
{
  delete _vbm;
}
