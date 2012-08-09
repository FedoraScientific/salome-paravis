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

#include "pqCustomChartDisplayPanelImplementation.h"
#include "pqCustomXYChartDisplayPanel.h"

#include "vtkSMProxy.h"

#include "pqDataRepresentation.h"
#include "pqPipelineSource.h"


pqCustomChartDisplayPanelImplementation::pqCustomChartDisplayPanelImplementation(QObject* p):
QObject(p)
    {
    }

bool pqCustomChartDisplayPanelImplementation::canCreatePanel(pqRepresentation* repr) const
{
  if(!repr || !repr->getProxy() || 
     (repr->getProxy()->GetXMLName() != QString("XYChartRepresentation")))
    {
      return false;
    }

  pqDataRepresentation* dataRepr = qobject_cast<pqDataRepresentation*>(repr);
  if(dataRepr)
    {
      pqPipelineSource* input = dataRepr->getInput(); 
      QString name = input->getProxy()->GetXMLName();
      if (name == "TableReader")
	{
	  return true;
	}
    }
  
  return false;
}

pqDisplayPanel* pqCustomChartDisplayPanelImplementation::createPanel(pqRepresentation* repr, QWidget* p)
{
  if(!repr || !repr->getProxy() ||
     (repr->getProxy()->GetXMLName() != QString("XYChartRepresentation")))
    {
      return NULL;
    }

  pqDataRepresentation* dataRepr = qobject_cast<pqDataRepresentation*>(repr);
  if(dataRepr)
    {
      pqPipelineSource* input = dataRepr->getInput(); 
      QString name=input->getProxy()->GetXMLName();

      if (name == "TableReader")
	{
	  return new pqCustomXYChartDisplayPanel(repr, p);
	}
    }
  
  return NULL;
}


