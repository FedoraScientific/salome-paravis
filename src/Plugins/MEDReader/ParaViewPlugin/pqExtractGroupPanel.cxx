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

#include "pqExtractGroupPanel.h"
#include "ui_ExtractGroupPanel.h"

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
#include "vtkExtractGroup.h"

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

class PixSingleExtractPanel
{
public:
  static const PixSingleExtractPanel &GetInstance();
  QPixmap getPixFromStr(int pos) const;
  PixSingleExtractPanel();
private:
  static const int NB_OF_DISCR=4;
  static PixSingleExtractPanel *UNIQUE_INSTANCE;
  QPixmap _pixmaps[NB_OF_DISCR];
};

PixSingleExtractPanel *PixSingleExtractPanel::UNIQUE_INSTANCE=0;

const PixSingleExtractPanel &PixSingleExtractPanel::GetInstance()
{
  if(!UNIQUE_INSTANCE)
    UNIQUE_INSTANCE=new PixSingleExtractPanel;
  return *UNIQUE_INSTANCE;
}

PixSingleExtractPanel::PixSingleExtractPanel()
{
  _pixmaps[0]=QPixmap(":/ParaViewResources/Icons/pqCellData16.png");
  _pixmaps[1]=QPixmap(":/ParaViewResources/Icons/pqPointData16.png");
}

QPixmap PixSingleExtractPanel::getPixFromStr(int pos) const
{
  if(pos>=0 && pos<=1)
    return _pixmaps[pos];
  else
    return QPixmap();
}

class pqExtractGroupPanel::pqUI: public QObject, public Ui::ExtractGroupPanel
{
public:
  pqUI(pqExtractGroupPanel* p):QObject(p)
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

pqExtractGroupPanel::pqExtractGroupPanel(pqProxy* object_proxy, QWidget* p):Superclass(object_proxy, p)
{
  this->UI=new pqUI(this);
  this->UI->setupUi(this);
  pqProxySILModel*proxyModel2 = new pqProxySILModel("GroupsFlagsStatusTree", &this->UI->SILModel);
  proxyModel2->setSourceModel(&this->UI->SILModel);
  this->UI->Fields->setHeaderHidden(true);
  this->updateSIL();
  this->linkServerManagerProperties();
  ////////////////////
  vtkSMProperty *SMProperty(this->proxy()->GetProperty("GroupsFlagsStatus"));
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
      if(st=="MeshesFamsGrps")
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
      QString meshName(QString::fromStdString((const char *)verticesNames2->GetValue(id1)));
      vtkAdjacentVertexIterator *it1(vtkAdjacentVertexIterator::New());
      g2->GetAdjacentVertices(id1,it1);
      vtkIdType idZeGrps(it1->Next());//zeGroups
      QList<QString> strs0; strs0.append(QString("Groups of \"%1\"").arg(meshName));
      pqTreeWidgetItemObject *item0(new pqTreeWidgetItemObject(this->UI->Fields,strs0));
      std::map<std::string,int> famIds(DeduceMapOfFamilyFromSIL(g2));
      //item0->setData(0,Qt::CheckStateRole,0);
      vtkAdjacentVertexIterator *itGrps(vtkAdjacentVertexIterator::New());
      g2->GetAdjacentVertices(idZeGrps,itGrps);
      while(itGrps->HasNext())
        {
          vtkIdType idg(itGrps->Next());
          QString name0(QString::fromStdString((const char *)verticesNames2->GetValue(idg))); QList<QString> strs0; strs0.append(name0);
          QString toolTipName0(name0);
          pqTreeWidgetItemObject *item1(new pqTreeWidgetItemObject(item0,strs0));
          //
          vtkAdjacentVertexIterator *itFamsOnGrp(vtkAdjacentVertexIterator::New());
          g2->GetAdjacentVertices(idg,itFamsOnGrp);
          bool isOnCell(true),isOnPoint(true);
          while(itFamsOnGrp->HasNext())
            {
              vtkIdType idfg(itFamsOnGrp->Next());
              std::string namefg((const char *)verticesNames2->GetValue(idfg));
              std::map<std::string,int>::const_iterator ittt(famIds.find(namefg));
              if(ittt==famIds.end())
                { isOnCell=false; isOnPoint=false; break; }
              int zeId((*ittt).second);
              if(zeId<0)
                {
                  if(!isOnCell)
                    { isOnCell=false; isOnPoint=false; break; }
                  else
                    isOnPoint=false;
                }
              if(zeId>0)
                {
                  if(!isOnPoint)
                    { isOnCell=false; isOnPoint=false; break; }
                  else
                    isOnCell=false;
                }
            }
          itFamsOnGrp->Delete();
          item1->setData(0,Qt::UserRole,name0);
          item1->setData(0,Qt::ToolTipRole,toolTipName0);
          item1->setData(0,Qt::CheckStateRole,0);
          if(isOnCell && !isOnPoint)
            item1->setData(0,Qt::DecorationRole,PixSingleExtractPanel::GetInstance().getPixFromStr(0));
          if(!isOnCell && isOnPoint)
            item1->setData(0,Qt::DecorationRole,PixSingleExtractPanel::GetInstance().getPixFromStr(1));
          this->propertyManager()->registerLink(item1,"checked",SIGNAL(checkedStateChanged(bool)),this->proxy(),SMProperty,ll++);
        }
      itGrps->Delete();
      // families 
      vtkIdType idZeFams(it1->Next());//zeFams
      strs0.clear(); strs0.append(QString("Families of \"%1\"").arg(meshName));
      pqTreeWidgetItemObject *item00(new pqTreeWidgetItemObject(this->UI->Fields,strs0));
      //item00->setData(0,Qt::CheckStateRole,0);
      vtkAdjacentVertexIterator *itFams(vtkAdjacentVertexIterator::New());
      g2->GetAdjacentVertices(idZeFams,itFams);
      while(itFams->HasNext())
        {
          vtkIdType idf(itFams->Next());
          std::string crudeFamName((const char *)verticesNames2->GetValue(idf));
          std::size_t pos(crudeFamName.find_first_of(ZE_SEP));
          std::string famName(crudeFamName.substr(0,pos)); std::string idStr(crudeFamName.substr(pos+strlen(ZE_SEP)));
          int idInt(QString(idStr.c_str()).toInt());
          famIds[famName]=idInt;
          QString name0(famName.c_str()); QList<QString> strs0; strs0.append(name0);
          QString toolTipName0(QString("%1 (%2)").arg(QString(famName.c_str())).arg(QString(idStr.c_str())));
          pqTreeWidgetItemObject *item1(new pqTreeWidgetItemObject(item00,strs0));
          item1->setData(0,Qt::UserRole,name0);
          item1->setData(0,Qt::ToolTipRole,toolTipName0);
          item1->setData(0,Qt::CheckStateRole,0);
          if(idInt<0)
            item1->setData(0,Qt::DecorationRole,PixSingleExtractPanel::GetInstance().getPixFromStr(0));
          if(idInt>0)
            item1->setData(0,Qt::DecorationRole,PixSingleExtractPanel::GetInstance().getPixFromStr(1));
          this->propertyManager()->registerLink(item1,"checked",SIGNAL(checkedStateChanged(bool)),this->proxy(),SMProperty,ll++);
        }
      itFams->Delete();
    }
  it0->Delete(); 
  this->UI->Fields->header()->setStretchLastSection(true);
  this->UI->Fields->expandAll();
  info->Delete();
  ////////////////////
  vtkSMProperty *SMPropertyExtractComp(this->proxy()->GetProperty("InsideOut"));
  this->propertyManager()->registerLink(this->UI->ExtractComplementary,"checked",SIGNAL(stateChanged(int)),this->proxy(),SMPropertyExtractComp);
  ////////////////////
  this->UI->VTKConnect->Connect(this->proxy(),vtkCommand::UpdateInformationEvent, this, SLOT(updateSIL()));
}

pqExtractGroupPanel::~pqExtractGroupPanel()
{
}

void pqExtractGroupPanel::linkServerManagerProperties()
{
  this->Superclass::linkServerManagerProperties();
}

void pqExtractGroupPanel::updateSIL()
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

void pqExtractGroupPanel::aLev4HasBeenFired()
{
  pqTreeWidgetItemObject *zeItem(qobject_cast<pqTreeWidgetItemObject *>(sender()));
  if(!zeItem)
    return;
  pqTreeWidgetItemObject *father(dynamic_cast<pqTreeWidgetItemObject *>(zeItem->QTreeWidgetItem::parent()));
  if(!father)
    return ;
  if(zeItem->isChecked())
    {
      // This part garantees that all leaves having not the same father than zeItem are desactivated
      foreach(pqTreeWidgetItemObject* elt,this->_all_lev4)
        {
          QTreeWidgetItem *testFath(elt->QTreeWidgetItem::parent());
          if(testFath!=father)
            if(elt->isChecked())
              elt->setChecked(false);
        }
      //If all leaves are checked the father is check too
      bool allItemsAreChked(true);
      for(int i=0;i<father->childCount() && allItemsAreChked;i++)
        {
          pqTreeWidgetItemObject *elt(dynamic_cast<pqTreeWidgetItemObject *>(father->child(i)));
          if(elt && !elt->isChecked())
            allItemsAreChked=false;
        }
      if(allItemsAreChked && !father->isChecked())
        father->setChecked(true);
    }
  else
    {
      // if all are unchecked - check it again
      bool allItemsAreUnChked(true);
      foreach(pqTreeWidgetItemObject* elt,this->_all_lev4)
        {
          if(elt && elt->isChecked())
            allItemsAreUnChked=false;
        }
      if(allItemsAreUnChked)
        zeItem->setChecked(true);// OK zeItem was required to be unchecked but as it is the last one. Recheck it !
      else
        {// if all items are uncheked inside a same parent - uncheck the parent
          allItemsAreUnChked=true;
          for(int i=0;i<father->childCount() && allItemsAreUnChked;i++)
            {
              pqTreeWidgetItemObject *elt(dynamic_cast<pqTreeWidgetItemObject *>(father->child(i)));
              if(elt && elt->isChecked())
            allItemsAreUnChked=false;
            }
          if(allItemsAreUnChked && father->isChecked())
            father->setChecked(false);
        } 
    }
}

std::map<std::string,int> pqExtractGroupPanel::DeduceMapOfFamilyFromSIL(vtkMutableDirectedGraph *graph)
{
  std::map<std::string,int> ret;
  int idNames(0);
  vtkAbstractArray *verticesNames(graph->GetVertexData()->GetAbstractArray("Names",idNames));
  vtkStringArray *verticesNames2(vtkStringArray::SafeDownCast(verticesNames));
  vtkAdjacentVertexIterator *it0(vtkAdjacentVertexIterator::New());
  vtkIdType id0;
  bool found(false);
  for(int i=0;i<verticesNames2->GetNumberOfValues();i++)
    {
      vtkStdString &st(verticesNames2->GetValue(i));
      if(st=="MeshesFamsGrps")
        {
          id0=i;
          found=true;
        }
    }
  if(!found)
    std::cerr << "There is an internal error ! The tree on server side has not the expected look !" << std::endl;
  graph->GetAdjacentVertices(id0,it0);
  while(it0->HasNext())
    {
      vtkIdType id1(it0->Next());//meshName
      vtkAdjacentVertexIterator *it1(vtkAdjacentVertexIterator::New());
      graph->GetAdjacentVertices(id1,it1);
      it1->Next();//zeGroups
      vtkIdType idZeFams(it1->Next());//zeFams
      vtkAdjacentVertexIterator *itFams(vtkAdjacentVertexIterator::New());
      graph->GetAdjacentVertices(idZeFams,itFams);
      while(itFams->HasNext())
        {
          vtkIdType idf(itFams->Next());
          std::string crudeFamName((const char *)verticesNames2->GetValue(idf));
          std::size_t pos(crudeFamName.find_first_of(ZE_SEP));
          std::string famName(crudeFamName.substr(0,pos)); std::string idStr(crudeFamName.substr(pos+strlen(ZE_SEP)));
          int idInt(QString(idStr.c_str()).toInt());
          ret[famName]=idInt;
        }
      it1->Delete();
    }
  it0->Delete();
  return ret;
}
