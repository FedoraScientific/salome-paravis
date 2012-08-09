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

#include "pqTableTo3DPanel.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

pqTableTo3DPanel::pqTableTo3DPanel(pqProxy* proxy, QWidget* p) :
  Superclass(proxy, p)
{
  QLineEdit* scaleFactor = this->findChild<QLineEdit*>("ScaleFactor");
  QCheckBox* useOptimusScale = this->findChild<QCheckBox*>("UseOptimusScale");
  QComboBox* presentationType = this->findChild<QComboBox*>("PresentationType");
  QWidget* numberContours = this->findChild<QWidget*>("NumberOfContours");
  
  if (scaleFactor && useOptimusScale)
    {
      this->connect(useOptimusScale, SIGNAL(toggled(bool)),
		    this, SLOT(onScaleModeChanged(bool)));
      onScaleModeChanged(useOptimusScale->isChecked());
    }

  if (presentationType && numberContours)
    {
      this->connect(presentationType, SIGNAL(currentIndexChanged(const QString &)),
		    this, SLOT(onPrsTypeChanged(const QString &)));
      onPrsTypeChanged(presentationType->currentText());
    }
}

pqTableTo3DPanel::~pqTableTo3DPanel()
{
}

void pqTableTo3DPanel::onScaleModeChanged(bool checked)
{
  QLineEdit* scaleFactor = this->findChild<QLineEdit*>("ScaleFactor");
  if (scaleFactor)
    {
      scaleFactor->setEnabled(!checked);
    }
}

void pqTableTo3DPanel::onPrsTypeChanged(const QString& type)
{
  QWidget* numberContours = this->findChild<QWidget*>("NumberOfContours");
  if (numberContours)
    {
      numberContours->setEnabled(type == "Contour");
    }
}
