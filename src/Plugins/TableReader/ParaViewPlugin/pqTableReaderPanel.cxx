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

#include "pqTableReaderPanel.h"

#include "vtkSMProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkProcessModule.h"

#include "pqPropertyLinks.h"

#include <QLayout>
#include <QSpinBox>


class pqTableReaderPanel::pqUI: public QObject, public Ui::TableReaderPanel
{
public:
  pqUI(pqTableReaderPanel* p) : QObject(p)
  {
  }

  ~pqUI()
  {
  }

  pqPropertyLinks Links;
};


pqTableReaderPanel::pqTableReaderPanel(pqProxy* proxy, QWidget* p) :
  Superclass(proxy, p)
{
  this->UI = new pqUI(this);
  this->UI->setupUi(this);
  
  this->linkServerManagerProperties();
  this->updateAvailableTables(false);

  this->connect(this->UI->ValueDelimiter, SIGNAL(textChanged(const QString&)),
		this, SLOT(onDelimiterChanged(const QString&)));
  this->connect(this->UI->TableNames, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onCurrentTableChanged(int)));
}

pqTableReaderPanel::~pqTableReaderPanel()
{
}

void pqTableReaderPanel::onCurrentTableChanged(int currentTableIndex)
{
  this->setModified();
}

void pqTableReaderPanel::onDelimiterChanged(const QString& value)
{
  this->updateAvailableTables(true);
}

void pqTableReaderPanel::linkServerManagerProperties()
{
  this->UI->Links.addPropertyLink(this->UI->TableNames, "currentIndex",
	  SIGNAL(currentIndexChanged(int)), this->proxy(), 
	  this->proxy()->GetProperty("TableNumber"));

  // To hook up the rest widgets
  this->Superclass::linkServerManagerProperties();
}

void pqTableReaderPanel::updateAvailableTables(const bool keepCurrent)
{
  vtkSMStringVectorProperty* prop = vtkSMStringVectorProperty::SafeDownCast(
				 this->proxy()->GetProperty("AvailableTables"));
  
  int currentIndex = this->UI->TableNames->currentIndex();
  QString currentText = this->UI->TableNames->currentText();

  this->UI->TableNames->clear();
  
  for(int id = 0; id < prop->GetNumberOfElements(); id++) 
    {
      QString text(prop->GetElement(id));
      if (text.isEmpty()) 
	{
	  text = QString("Table:%1").arg(id);
	}
      this->UI->TableNames->addItem(text);
    }

  if (keepCurrent && 
      currentIndex < this->UI->TableNames->maxCount() &&
      currentText == this->UI->TableNames->itemText(currentIndex)) 
    {
      this->UI->TableNames->setCurrentIndex(currentIndex);
    }
}
