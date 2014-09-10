// PARAVIS : ParaView wrapper SALOME module
//
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
// File   : PVGUI_ParaViewSettingsPane.cxx
// Author : Vitaly Smetannikov
//

#include "PVGUI_ParaViewSettingsPane.h"
#include "ui_pqOptionsDialog.h"

#include <pqApplicationSettingsReaction.h>

#include <QString>


//class OptionsDialogModelItem
//{
//public:
//  OptionsDialogModelItem();
//  OptionsDialogModelItem(const QString &name);
//  ~OptionsDialogModelItem();
//
//  OptionsDialogModelItem *Parent;
//  QString Name;
//  QList<OptionsDialogModelItem *> Children;
//};
//
//
//class OptionsDialogModel : public QAbstractItemModel
//{
//public:
//  OptionsDialogModel(QObject *parent=0);
//  virtual ~OptionsDialogModel();
//
//  virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
//  virtual int columnCount(const QModelIndex &parent=QModelIndex()) const;
//  virtual QModelIndex index(int row, int column,
//      const QModelIndex &parent=QModelIndex()) const;
//  virtual QModelIndex parent(const QModelIndex &child) const;
//
//  virtual QVariant data(const QModelIndex &index,
//      int role=Qt::DisplayRole) const;
//
//  QModelIndex getIndex(const QString &path) const;
//  QString getPath(const QModelIndex &index) const;
//  void addPath(const QString &path);
//  bool removeIndex(const QModelIndex &index);
//
//private:
//  QModelIndex getIndex(OptionsDialogModelItem *item) const;
//
//private:
//  OptionsDialogModelItem *Root;
//};


class OptionsDialogForm : public Ui::PVGUIOptionsFrame
{
public:
  OptionsDialogForm();
  ~OptionsDialogForm();

//  QMap<QString, pqOptionsPage *> Pages;
//  OptionsDialogModel *Model;
//  int ApplyUseCount;
  //bool ApplyNeeded;
};


//----------------------------------------------------------------------------
//OptionsDialogModelItem::OptionsDialogModelItem()
//  : Name(), Children()
//{
//  this->Parent = 0;
//}
//
//OptionsDialogModelItem::OptionsDialogModelItem(const QString &name)
//  : Name(name), Children()
//{
//  this->Parent = 0;
//}
//
//OptionsDialogModelItem::~OptionsDialogModelItem()
//{
//  QList<OptionsDialogModelItem *>::Iterator iter = this->Children.begin();
//  for( ; iter != this->Children.end(); ++iter)
//    {
//    delete *iter;
//    }
//}


//----------------------------------------------------------------------------
//OptionsDialogModel::OptionsDialogModel(QObject *parentObject)
//  : QAbstractItemModel(parentObject)
//{
//  this->Root = new OptionsDialogModelItem();
//}
//
//OptionsDialogModel::~OptionsDialogModel()
//{
//  delete this->Root;
//}
//
//int OptionsDialogModel::rowCount(const QModelIndex &parentIndex) const
//{
//  OptionsDialogModelItem *item = this->Root;
//  if(parentIndex.isValid())
//    {
//    item = reinterpret_cast<OptionsDialogModelItem *>(
//        parentIndex.internalPointer());
//    }
//
//  return item->Children.size();
//}
//
//int OptionsDialogModel::columnCount(const QModelIndex &) const
//{
//  return 1;
//}
//
//QModelIndex OptionsDialogModel::index(int row, int column,
//    const QModelIndex &parentIndex) const
//{
//  OptionsDialogModelItem *item = this->Root;
//  if(parentIndex.isValid())
//    {
//    item = reinterpret_cast<OptionsDialogModelItem *>(
//        parentIndex.internalPointer());
//    }
//
//  if(column == 0 && row >= 0 && row < item->Children.size())
//    {
//    return this->createIndex(row, column, item->Children[row]);
//    }
//
//  return QModelIndex();
//}
//
//QModelIndex OptionsDialogModel::parent(const QModelIndex &child) const
//{
//  if(child.isValid())
//    {
//    OptionsDialogModelItem *item =
//        reinterpret_cast<OptionsDialogModelItem *>(child.internalPointer());
//    return this->getIndex(item->Parent);
//    }
//
//  return QModelIndex();
//}
//
//QVariant OptionsDialogModel::data(const QModelIndex &idx, int role) const
//{
//  if(idx.isValid())
//    {
//    OptionsDialogModelItem *item =
//        reinterpret_cast<OptionsDialogModelItem *>(idx.internalPointer());
//    if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
//      {
//      return QVariant(item->Name);
//      }
//    }
//
//  return QVariant();
//}
//
//QModelIndex OptionsDialogModel::getIndex(const QString &path) const
//{
//  OptionsDialogModelItem *item = this->Root;
//  QStringList names = path.split(".");
//  QStringList::Iterator iter = names.begin();
//  for( ; item && iter != names.end(); ++iter)
//    {
//    OptionsDialogModelItem *child = 0;
//    QList<OptionsDialogModelItem *>::Iterator jter = item->Children.begin();
//    for( ; jter != item->Children.end(); ++jter)
//      {
//      if((*jter)->Name == *iter)
//        {
//        child = *jter;
//        break;
//        }
//      }
//
//    item = child;
//    }
//
//  if(item && item != this->Root)
//    {
//    return this->getIndex(item);
//    }
//
//  return QModelIndex();
//}
//
//QString OptionsDialogModel::getPath(const QModelIndex &idx) const
//{
//  if(idx.isValid())
//    {
//    QString path;
//    OptionsDialogModelItem *item =
//        reinterpret_cast<OptionsDialogModelItem *>(idx.internalPointer());
//    if(item)
//      {
//      path = item->Name;
//      item = item->Parent;
//      }
//
//    while(item && item != this->Root)
//      {
//      path.prepend(".").prepend(item->Name);
//      item = item->Parent;
//      }
//
//    return path;
//    }
//
//  return QString();
//}
//
//void OptionsDialogModel::addPath(const QString &path)
//{
//  OptionsDialogModelItem *item = this->Root;
//  QStringList names = path.split(".");
//  QStringList::Iterator iter = names.begin();
//  for( ; iter != names.end(); ++iter)
//    {
//    OptionsDialogModelItem *child = 0;
//    QList<OptionsDialogModelItem *>::Iterator jter = item->Children.begin();
//    for( ; jter != item->Children.end(); ++jter)
//      {
//      if((*jter)->Name == *iter)
//        {
//        child = *jter;
//        break;
//        }
//      }
//
//    if(!child)
//      {
//      child = new OptionsDialogModelItem(*iter);
//      child->Parent = item;
//      QModelIndex parentIndex = this->getIndex(item);
//      int row = item->Children.size();
//      this->beginInsertRows(parentIndex, row, row);
//      item->Children.append(child);
//      this->endInsertRows();
//      }
//
//    item = child;
//    }
//}
//
//bool OptionsDialogModel::removeIndex(const QModelIndex &idx)
//{
//  if(idx.isValid())
//    {
//    OptionsDialogModelItem *item =
//        reinterpret_cast<OptionsDialogModelItem *>(idx.internalPointer());
//    if(item->Children.size() == 0)
//      {
//      QModelIndex parentIndex = this->getIndex(item->Parent);
//      this->beginRemoveRows(parentIndex, idx.row(), idx.row());
//      item->Parent->Children.removeAt(idx.row());
//      this->endRemoveRows();
//      delete item;
//      return true;
//      }
//    }
//
//  return false;
//}
//
//QModelIndex OptionsDialogModel::getIndex(
//    OptionsDialogModelItem *item) const
//{
//  if(item && item->Parent)
//    {
//    return this->createIndex(item->Parent->Children.indexOf(item), 0, item);
//    }
//
//  return QModelIndex();
//}


//----------------------------------------------------------------------------
OptionsDialogForm::OptionsDialogForm()
  : Ui::PVGUIOptionsFrame()
{
//  this->Model = new OptionsDialogModel();
//  this->ApplyUseCount = 0;
//  //this->ApplyNeeded = false;
}

OptionsDialogForm::~OptionsDialogForm()
{
//  delete this->Model;
}


//----------------------------------------------------------------------------
PVGUI_ParaViewSettingsPane::PVGUI_ParaViewSettingsPane(QWidget *widgetParent)
  : QtxUserDefinedContent(widgetParent)
{
  Form = new OptionsDialogForm();
  Form->setupUi(this);
  
  // Connect the button to the standard ParaView 4.2 setting reaction for now:
  connect(Form->pvButton, SIGNAL(clicked()),
          this, SLOT(onRequestParaviewSettings()));

  
////  this->Form->PageNames->setModel(this->Form->Model);
//
//  // Hide the tree widget header view.
//  this->Form->PageNames->header()->hide();
//
//  // Hide the apply and reset buttons until they are needed.
//  //this->Form->ApplyButton->setEnabled(false);
//  //this->Form->ResetButton->setEnabled(false);
//  //this->Form->ApplyButton->hide();
//  //this->Form->ResetButton->hide();
//
//  this->connect(this->Form->PageNames->selectionModel(),
//      SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
//      this, SLOT(changeCurrentPage()));
//  //this->connect(this->Form->ApplyButton, SIGNAL(clicked()),
//  //    this, SLOT(applyChanges()));
//  //this->connect(this->Form->ResetButton, SIGNAL(clicked()),
//  //    this, SLOT(resetChanges()));
//  //this->connect(this->Form->CloseButton, SIGNAL(clicked()),
//  //    this, SLOT(accept()));
//
//  // Code From pqApplicationOptionsDialog
//  //this->setApplyNeeded(true);
//
//  pqApplicationOptions* appOptions = new pqApplicationOptions;
//  this->addOptions(appOptions);
//
//  pqGlobalRenderViewOptions* renOptions = new pqGlobalRenderViewOptions;
//  this->addOptions(renOptions);
//
//  QStringList pages = appOptions->getPageList();
//  if(pages.size())
//    {
//    this->setCurrentPage(pages[0]);
//    }
//
//  /// Add panes as plugins are loaded.
//  QObject::connect(pqApplicationCore::instance()->getPluginManager(),
//    SIGNAL(guiInterfaceLoaded(QObject*)),
//    this, SLOT(pluginLoaded(QObject*)));
//
//  // Load panes from already loaded plugins.
//  foreach (QObject* plugin_interface, pqApplicationCore::instance()->interfaceTracker()->interfaces())
//           //pqApplicationCore::instance()->getPluginManager()->interfaces())
//    {
//    this->pluginLoaded(plugin_interface);
//    }
}

PVGUI_ParaViewSettingsPane::~PVGUI_ParaViewSettingsPane()
{
  delete this->Form;
}

void PVGUI_ParaViewSettingsPane::onRequestParaviewSettings()
{
  pqApplicationSettingsReaction::showApplicationSettingsDialog();
}

/*
bool PVGUI_ParaViewSettingsPane::isApplyNeeded() const
{
  return this->Form->ApplyNeeded;
}
*/
/*
void PVGUI_ParaViewSettingsPane::setApplyNeeded(bool applyNeeded)
{
  if(applyNeeded != this->Form->ApplyNeeded)
    {
    if(!applyNeeded)
      {
      this->Form->ApplyNeeded = false;
      //this->Form->ApplyButton->setEnabled(false);
      //this->Form->ResetButton->setEnabled(false);
      }
    else if(this->Form->ApplyUseCount > 0)
      {
      this->Form->ApplyNeeded = true;
      //this->Form->ApplyButton->setEnabled(true);
      //this->Form->ResetButton->setEnabled(true);
      }
    }
}
*/
//void PVGUI_ParaViewSettingsPane::addOptions(const QString &path, pqOptionsPage *options)
//{
//  if(!options)
//    {
//    return;
//    }
//
//  // See if the page is a container.
//  pqOptionsContainer *container = qobject_cast<pqOptionsContainer *>(options);
//  if(!container && path.isEmpty())
//    {
//    return;
//    }
//
//  // See if the page/container uses the apply button.
//  if(options->isApplyUsed())
//    {
//    this->Form->ApplyUseCount++;
//    /*if(this->Form->ApplyUseCount == 1)
//      {
//        //this->Form->ApplyButton->show();
//        //this->Form->ResetButton->show();
//        //QObject::connect(this, SIGNAL(accepted()), this, SLOT(applyChanges()));
//        }*/
//
//    //this->connect(options, SIGNAL(changesAvailable()),
//    //this, SLOT(enableButtons()));
//    }
//
//  // Add the widget to the stack.
//  this->Form->Stack->addWidget(options);
//
//  // Add the page(s) to the map and the model.
//  if(container)
//    {
//    // If the path is not empty, use it as the page prefix.
//    QString prefix;
//    if(!path.isEmpty())
//      {
//      prefix = path;
//      prefix.append(".");
//      }
//
//    container->setPagePrefix(prefix);
//
//    // Get the list of pages from the container.
//    QStringList pathList = container->getPageList();
//    QStringList::Iterator iter = pathList.begin();
//    for( ; iter != pathList.end(); ++iter)
//      {
//      this->Form->Pages.insert(prefix + *iter, options);
//      this->Form->Model->addPath(prefix + *iter);
//      }
//    }
//  else
//    {
//    this->Form->Pages.insert(path, options);
//    this->Form->Model->addPath(path);
//    }
//}
//
//void PVGUI_ParaViewSettingsPane::addOptions(pqOptionsContainer *options)
//{
//  this->addOptions(QString(), options);
//}
//
//void PVGUI_ParaViewSettingsPane::removeOptions(pqOptionsPage *options)
//{
//  if(!options)
//    {
//    return;
//    }
//
//  // Remove the widget from the stack.
//  this->Form->Stack->removeWidget(options);
//
//  // See if the options use the apply button.
//  if(options->isApplyUsed())
//    {
//    this->Form->ApplyUseCount--;
//    /*if(this->Form->ApplyUseCount == 0)
//      {
//        //this->Form->ApplyNeeded = false;
//      //this->Form->ApplyButton->setEnabled(false);
//      //this->Form->ResetButton->setEnabled(false);
//      //this->Form->ApplyButton->hide();
//      //this->Form->ResetButton->hide();
//      //QObject::disconnect(this, SIGNAL(accepted()), this, SLOT(applyChanges()));
//      }*/
//
//    this->disconnect(options, 0, this, 0);
//    }
//
//  // Search the map for the paths to remove.
//  QMap<QString, pqOptionsPage *>::Iterator iter = this->Form->Pages.begin();
//  while(iter != this->Form->Pages.end())
//    {
//    if(*iter == options)
//      {
//      QString path = iter.key();
//      iter = this->Form->Pages.erase(iter);
//
//      // Remove the item from the tree model as well.
//      QModelIndex index = this->Form->Model->getIndex(path);
//      QPersistentModelIndex parentIndex = index.parent();
//      if(this->Form->Model->removeIndex(index))
//        {
//        // Remove any empty parent items.
//        while(parentIndex.isValid() &&
//            this->Form->Model->rowCount(parentIndex) == 0)
//          {
//          index = parentIndex;
//          parentIndex = index.parent();
//
//          // Make sure the index path isn't in the map.
//          path = this->Form->Model->getPath(index);
//          if(this->Form->Pages.find(path) == this->Form->Pages.end())
//            {
//            if(!this->Form->Model->removeIndex(index))
//              {
//              break;
//              }
//            }
//          }
//        }
//      }
//    else
//      {
//      ++iter;
//      }
//    }
//}
//
//void PVGUI_ParaViewSettingsPane::setCurrentPage(const QString &path)
//{
//  QModelIndex current = this->Form->Model->getIndex(path);
//  this->Form->PageNames->setCurrentIndex(current);
//}

void PVGUI_ParaViewSettingsPane::applyChanges()
{
  //if(this->Form->ApplyNeeded)
  //  {
//    BEGIN_UNDO_SET("Changed View Settings");
//    emit this->aboutToApplyChanges();
//    QMap<QString, pqOptionsPage *>::Iterator iter = this->Form->Pages.begin();
//    for( ; iter != this->Form->Pages.end(); ++iter)
//      {
//      (*iter)->applyChanges();
//      }
//
//    //this->setApplyNeeded(false);
//    emit this->appliedChanges();
//    END_UNDO_SET();
    //}
}

void PVGUI_ParaViewSettingsPane::resetChanges()
{
  //if(this->Form->ApplyNeeded)
  //{
//    QMap<QString, pqOptionsPage *>::Iterator iter = this->Form->Pages.begin();
//    for( ; iter != this->Form->Pages.end(); ++iter)
//      {
//      (*iter)->resetChanges();
//      }

    //this->setApplyNeeded(false);
    //}
}

//void PVGUI_ParaViewSettingsPane::changeCurrentPage()
//{
//  // Get the current index from the view.
//  QModelIndex current = this->Form->PageNames->currentIndex();
//
//  // Look up the path for the current index.
//  QString path = this->Form->Model->getPath(current);
//  QMap<QString, pqOptionsPage *>::Iterator iter = this->Form->Pages.find(path);
//  if(iter == this->Form->Pages.end())
//    {
//    // If no page is found, show the blank page.
//    this->Form->Stack->setCurrentWidget(this->Form->BlankPage);
//    }
//  else
//    {
//    this->Form->Stack->setCurrentWidget(*iter);
//    pqOptionsContainer *container = qobject_cast<pqOptionsContainer *>(*iter);
//    if(container)
//      {
//      // Get the path prefix from the container.
//      QString prefix = container->getPagePrefix();
//      if(!prefix.isEmpty())
//        {
//        // Remove the prefix from the path.
//        path.remove(0, prefix.length());
//        }
//
//      // Set the page on the container object.
//      container->setPage(path);
//      }
//    }
//}
//
////-----------------------------------------------------------------------------
//void PVGUI_ParaViewSettingsPane::pluginLoaded(QObject* iface)
//{
//  pqViewOptionsInterface* viewOptions =
//    qobject_cast<pqViewOptionsInterface*>(iface);
//  if (viewOptions)
//    {
//    foreach(QString viewtype, viewOptions->viewTypes())
//      {
//      // Try to create global view options
//      pqOptionsContainer* globalOptions =
//        viewOptions->createGlobalViewOptions(viewtype, this);
//      if (globalOptions)
//        {
//        this->addOptions(globalOptions);
//        }
//      }
//    }
//}

void PVGUI_ParaViewSettingsPane::store(QtxResourceMgr* , QtxPreferenceMgr* )
{
  applyChanges();
}

void PVGUI_ParaViewSettingsPane::retrieve(QtxResourceMgr* , QtxPreferenceMgr* )
{
}

