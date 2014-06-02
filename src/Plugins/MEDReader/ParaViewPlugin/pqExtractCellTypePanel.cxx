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

#include "pqExtractCellTypePanel.h"
#include "ui_ExtractCellTypePanel.h"

#include "vtkProcessModule.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"
#include "vtkIntArray.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkPVSILInformation.h"
#include "vtkGraph.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkAdjacentVertexIterator.h"
#include "vtkSMPropertyHelper.h"
#include "vtkStringArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkExtractCellType.h"

#include "pqTreeWidgetItemObject.h"
#include "pqSMAdaptor.h"
#include "pqProxy.h"
#include "pqPropertyManager.h"
#include "pqSILModel.h"
#include "pqProxySILModel.h"
#include "pqTreeViewSelectionHelper.h"
#include "pqTreeWidgetSelectionHelper.h"
#include "pqPropertyLinks.h"

#include <QHeaderView>

static const char ZE_SEP[]="@@][@@";

class pqExtractCellTypePanel::pqUI: public QObject, public Ui::ExtractCellTypePanel
{
public:
  pqUI(pqExtractCellTypePanel* p):QObject(p)
  {
    this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    this->SILUpdateStamp = -1;
  }

  ~pqUI() { }

  pqSILModel SILModel;
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
  pqPropertyLinks Links;
  QMap<QTreeWidgetItem*, QString> TreeItemToPropMap;
  int SILUpdateStamp;
};

pqExtractCellTypePanel::pqExtractCellTypePanel(pqProxy* object_proxy, QWidget* p):Superclass(object_proxy, p)
{
  this->UI=new pqUI(this);
  this->UI->setupUi(this);
  pqProxySILModel*proxyModel2 = new pqProxySILModel("GeoTypesStatusTree", &this->UI->SILModel);
  proxyModel2->setSourceModel(&this->UI->SILModel);
  this->UI->Fields->setHeaderHidden(true);
  this->updateSIL();
  this->linkServerManagerProperties();
  ////////////////////
  vtkSMProperty *SMProperty(this->proxy()->GetProperty("GeoTypesStatus"));
  ////////////////////
  vtkSMProxy* reader = this->referenceProxy()->getProxy();
  vtkPVSILInformation* info=vtkPVSILInformation::New();
  reader->GatherInformation(info);
  vtkGraph *g(info->GetSIL());
  //vtkMutableDirectedGraph *g2(vtkMutableDirectedGraph::SafeDownCast(g));// agy: this line does not work in client/server mode ! but it works in standard mode ! Don't know why. ParaView bug ?
  vtkMutableDirectedGraph *g2(static_cast<vtkMutableDirectedGraph *>(g));
  int idNames(0);
  vtkAbstractArray *verticesNames(g2->GetVertexData()->GetAbstractArray("Names",idNames));
  vtkStringArray *verticesNames2(vtkStringArray::SafeDownCast(verticesNames));
  vtkIdType id0;
  bool found(false);
  for(int i=0;i<verticesNames2->GetNumberOfValues();i++)
    {
      vtkStdString &st(verticesNames2->GetValue(i));
      if(st=="CellTypesTree")
        {
          id0=i;
          found=true;
        }
    }
  if(!found)
    std::cerr << "There is an internal error ! The tree on server side has not the expected look !" << std::endl;
  vtkAdjacentVertexIterator *it0(vtkAdjacentVertexIterator::New());
  g2->GetAdjacentVertices(id0,it0);
  int kk(0),ll(0);
  while(it0->HasNext())
    {
      vtkIdType id1(it0->Next());
      QString geoTypeName(QString::fromStdString((const char *)verticesNames2->GetValue(id1)));
      vtkAdjacentVertexIterator *it1(vtkAdjacentVertexIterator::New());
      g2->GetAdjacentVertices(id1,it1);
      QList<QString> strs0; strs0.append(QString("%1").arg(geoTypeName));
      pqTreeWidgetItemObject *item0(new pqTreeWidgetItemObject(this->UI->Fields,strs0));
      vtkAdjacentVertexIterator *itGrps(vtkAdjacentVertexIterator::New());
      g2->GetAdjacentVertices(id1,itGrps);
      vtkIdType idg(itGrps->Next());
      QString name0(QString::fromStdString((const char *)verticesNames2->GetValue(idg)));
      QString toolTipName0(QString("%1 (vtkId=%2)").arg(geoTypeName).arg(name0));
      item0->setData(0,Qt::UserRole,geoTypeName);
      item0->setData(0,Qt::ToolTipRole,toolTipName0);
      item0->setData(0,Qt::CheckStateRole,0);
      this->propertyManager()->registerLink(item0,"checked",SIGNAL(checkedStateChanged(bool)),this->proxy(),SMProperty,ll++);
    }
  it0->Delete(); 
  this->UI->Fields->header()->setStretchLastSection(true);
  this->UI->Fields->expandAll();
  info->Delete();
  ////////////////////
  vtkSMProperty *SMPropertyExtractComp(this->proxy()->GetProperty("InsideOut"));
  this->propertyManager()->registerLink(this->UI->ExtractComplementary,"checked",SIGNAL(stateChanged(int)),this->proxy(),SMPropertyExtractComp);
  ////////////////////
  this->UI->VTKConnect->Connect(this->proxy(),vtkCommand::UpdateInformationEvent,this,SLOT(updateSIL()));
}

pqExtractCellTypePanel::~pqExtractCellTypePanel()
{
}

void pqExtractCellTypePanel::linkServerManagerProperties()
{
  this->Superclass::linkServerManagerProperties();
}

void pqExtractCellTypePanel::updateSIL()
{
  vtkSMProxy* reader = this->referenceProxy()->getProxy();
  reader->UpdatePropertyInformation(reader->GetProperty("SILUpdateStamp"));
  int stamp = vtkSMPropertyHelper(reader, "SILUpdateStamp").GetAsInt();
  if (stamp != this->UI->SILUpdateStamp)
    {
      this->UI->SILUpdateStamp = stamp;
      vtkPVSILInformation* info = vtkPVSILInformation::New();
      reader->GatherInformation(info);
      this->UI->SILModel.update(info->GetSIL());
      this->UI->Fields->expandAll();
      info->Delete();
    }
}
