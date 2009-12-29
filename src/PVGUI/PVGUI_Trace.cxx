#include "PVGUI_Trace.h"
#include "PVGUI_Module.h"

#include <vtkSMPythonTraceObserver.h>
#include <vtkSMSourceProxy.h>
#include <vtkSMViewProxy.h>
#include <vtkSMProxy.h>
#include <vtkSMProperty.h>
#include <vtkSMProxyProperty.h>
#include <vtkSMProxyIterator.h>
#include <pqServer.h>
#include <vtkSMProxyManager.h>
#include <vtkSMDomain.h>
#include <vtkSMProxyListDomain.h>
#include <vtkSMProxyGroupDomain.h>
#include <vtkSMPropertyIterator.h>
#include <vtkCallbackCommand.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkSMIntVectorProperty.h>
#include <vtkSMIdTypeVectorProperty.h>
#include <vtkSMDoubleVectorProperty.h>
#include <vtkSMEnumerationDomain.h>
#include <vtkSMDomainIterator.h>
#include <vtkSMInputProperty.h>

#include <QFile>
#include <QTextStream>

extern PVGUI_Module* ParavisModule;

static trace_globals globals;
static bool myInitialised = false;


//********************************************************************
QString pyvariable_from_proxy_name(const QString& proxy_name)
{
  QString aName = proxy_name;
  return make_name_valid(aName.replace(".", "_"));
}

//********************************************************************
proxy_trace_info::proxy_trace_info(vtkSMProxy* proxy, 
                                   const QString& proxyGroup, 
                                   const QString& proxyName) 
{
  Proxy = proxy;
  Group = proxyGroup;
  ProxyName = proxyName;
  PyVariable = pyvariable_from_proxy_name(proxyName);
  ignore_next_unregister = false;
}
  
//********************************************************************
prop_trace_info::prop_trace_info(const proxy_trace_info_ptr proxyTraceInfo, vtkSMProperty* prop) 
{
  Prop = prop;
  // Determine python variable name
  PyVariable = prop->GetXMLLabel();
  // For non-self properties, use the xml name instead of xml label:
  if (prop->GetParent() != proxyTraceInfo->Proxy)
    PyVariable = proxyTraceInfo->Proxy->GetPropertyName(prop);
  PyVariable = make_name_valid(PyVariable);
}


//********************************************************************
QString make_name_valid(const QString& theName)
{
  QString name = theName;
  if (name.isNull())
    return QString("");
  if ((name.indexOf('(') >= 0) || (name.indexOf(')') >=0))
    return QString("");
  name = name.replace(" ","");
  name = name.replace("-","");
  name = name.replace(":","");
  name = name.replace(".","");
  if (!name.at(0).isLetter())
    name = "a" + name;
  return name;
}

//********************************************************************
vtkSMPythonTraceObserver* trace_observer()
{
  return globals.observer;
}

//********************************************************************
QStringList ignoredViewProperties()
{
  return globals.ignored_view_properties;
}

//********************************************************************
QStringList ignoredRepresentationProperties()
{
  return globals.ignored_representation_properties;
}

//********************************************************************
bool propIsIgnored(const proxy_trace_info_ptr info, const QString& propName)
{
  QStringList aIgnoredList = ignoredViewProperties();
  if ((info->Group == "views") && (aIgnoredList.contains(propName))) 
    return true;
  aIgnoredList = ignoredRepresentationProperties();
  if ((info->Group == "representations") && (aIgnoredList.contains(propName)))
    return true;
  return false;
}

//********************************************************************
bool operator < (const QStringList& list1, const QStringList& list2) {
  QString s1 = list1.join(":");
  QString s2 = list2.join(":");
  return (s1 < s2);
}

//********************************************************************
void GetProxiesInGroup(const char* theName, QMap<QStringList,vtkSMProxy*>& proxies) 
{
  pqServer* aServer = ParavisModule->getActiveServer();

  vtkSMProxyIterator* aIt = vtkSMProxyIterator::New();
  aIt->SetConnectionID(aServer->GetConnectionID());
  aIt->SetModeToOneGroup();
  for (aIt->Begin(theName); !aIt->IsAtEnd(); aIt->Next()) {
    vtkSMProxy* aProxy = aIt->GetProxy();
    QStringList aKey;
    aKey << aIt->GetKey() << aProxy->GetSelfIDAsString();
    proxies[aKey] = aProxy;
  }
  aIt->Delete();
}

//********************************************************************
void track_existing_sources()
{
  QMap<QStringList, vtkSMProxy*> existing_sources;
  GetProxiesInGroup("sources", existing_sources);

  QMap<QStringList, vtkSMProxy*>::const_iterator aIt;
  for (aIt = existing_sources.constBegin(); aIt!=existing_sources.constEnd(); ++aIt) {
    QStringList aKey = aIt.key();
    vtkSMProxy* aProxy = aIt.value();
    track_existing_source_proxy(aProxy, "");
  }
}

//********************************************************************
proxy_trace_info_ptr track_existing_source_proxy(vtkSMProxy* proxy, const QString& proxy_name)
{
  proxy_trace_info_ptr proxy_info(new proxy_trace_info(proxy, "sources", proxy_name));
  globals.registered_proxies.append(proxy_info);
  if ((!globals.last_active_source) && (proxy == globals.active_source_at_start)) {
    globals.last_active_source = proxy;
    globals.trace_output.append(QString("%1 = GetActiveSource()").arg(proxy_info->PyVariable));
  } else {
    globals.trace_output.append(QString("%1 = FindSource(\"%2\")").arg(proxy_info->PyVariable).arg(proxy_name));
  }
  return proxy_info;
}

//********************************************************************
QList<vtkSMProxy*> GetRenderViews() 
{
  QList<vtkSMProxy*> render_modules;

  pqServer* aServer = ParavisModule->getActiveServer();

  vtkSMProxyIterator* aIt = vtkSMProxyIterator::New();
  aIt->SetConnectionID(aServer->GetConnectionID());
  for (aIt->Begin(); !aIt->IsAtEnd(); aIt->Next()) {
    vtkSMProxy* aProxy = aIt->GetProxy();
    if (aProxy->IsA("vtkSMRenderViewProxy"))
      render_modules.append(aProxy);
  }
  aIt->Delete();
  return render_modules;
}

//********************************************************************
proxy_trace_info_ptr track_existing_view_proxy(vtkSMProxy* proxy, const QString& proxy_name)
{
  proxy_trace_info_ptr proxy_info(new proxy_trace_info(proxy, "views", proxy_name));
  globals.registered_proxies.append(proxy_info);
  if ((!globals.last_active_view) && (proxy == globals.active_view_at_start)) {
    globals.last_active_view = proxy;
    globals.trace_output.append(QString("%1 = GetRenderView()").arg(proxy_info->PyVariable));
  } else {
    QList<vtkSMProxy*> all_views = GetRenderViews();
    if (all_views.contains(proxy)) {
      int view_index = all_views.indexOf(proxy);
      if (view_index != -1)
        globals.trace_output.append(QString("%1 = GetRenderViews()[%2]").arg(proxy_info->PyVariable).arg(view_index));
    }
  }
  return proxy_info;
}

//********************************************************************
proxy_trace_info_ptr track_existing_representation_proxy(vtkSMProxy* proxy, 
                                                         const QString& proxy_name)
{
  vtkSMProxyProperty* input_property = dynamic_cast<vtkSMProxyProperty*>(proxy->GetProperty("Input"));
  if (!input_property) return proxy_trace_info_ptr();
  
  if (input_property->GetNumberOfProxies() > 0) {
    vtkSMProxy* input_proxy = input_property->GetProxy(0);
    proxy_trace_info_ptr input_proxy_info = get_proxy_info(input_proxy);
    if (input_proxy_info.get()) {
      proxy_trace_info_ptr proxy_info(new proxy_trace_info(proxy, "representations", proxy_name));
      globals.registered_proxies.append(proxy_info);
      globals.trace_output.append(QString("%1 = GetDisplayProperties(%2)").arg(proxy_info->PyVariable).arg(input_proxy_info->PyVariable));
      return proxy_info;
    }
  }
  return proxy_trace_info_ptr();
}

//********************************************************************
proxy_trace_info_ptr track_existing_proxy(vtkSMProxy* proxy)
{
  QString proxy_name = get_source_proxy_registration_name(proxy);
  if (!proxy_name.isNull())
    return track_existing_source_proxy(proxy, proxy_name);

  proxy_name = get_representation_proxy_registration_name(proxy);
  if (!proxy_name.isNull())
    return track_existing_representation_proxy(proxy, proxy_name);

  proxy_name = get_view_proxy_registration_name(proxy);
  if (!proxy_name.isNull())
    return track_existing_view_proxy(proxy, proxy_name);
  return proxy_trace_info_ptr();
}

//********************************************************************
proxy_trace_info_ptr get_proxy_info(const QString& p, bool search_existing)
{
  foreach(proxy_trace_info_ptr info, globals.last_registered_proxies) {
    if (info->PyVariable == p) return info;
  }
  foreach(proxy_trace_info_ptr info, globals.registered_proxies) {
    if (info->PyVariable == p) return info;
  }

  if(search_existing) return proxy_trace_info_ptr();

  return proxy_trace_info_ptr();
}

//********************************************************************
proxy_trace_info_ptr get_proxy_info(vtkSMProxy* p, bool search_existing)
{
  foreach(proxy_trace_info_ptr info, globals.last_registered_proxies) {
    if (info->Proxy == p) return info;
  }
  foreach(proxy_trace_info_ptr info, globals.registered_proxies) {
    if (info->Proxy == p) return info;
  }
  // It must be a proxy that existed before trace started
  if (search_existing) return track_existing_proxy(p);

  return proxy_trace_info_ptr();
}

//********************************************************************
void ensure_active_source(const proxy_trace_info_ptr proxy_info)
{
  if (proxy_info && (proxy_info->Proxy != globals.last_active_source)) {
    globals.trace_output.append(QString("SetActiveSource(%1)").arg(proxy_info->PyVariable));
    globals.last_active_source = proxy_info->Proxy;
  }
}

//********************************************************************
void ensure_active_view(const proxy_trace_info_ptr proxy_info)
{
  if (proxy_info && (proxy_info->Proxy != globals.last_active_view)) {
    globals.trace_output.append(QString("SetActiveView(%1)").arg(proxy_info->PyVariable));
    globals.last_active_view = proxy_info->Proxy;
  }
}

//********************************************************************
proxy_trace_info_ptr get_input_proxy_info_for_rep(const proxy_trace_info_ptr rep_info)
{
  if (rep_info->CurrentProps.contains("Input")) {
    QString input_proxy_pyvariable = rep_info->CurrentProps["Input"];
    return get_proxy_info(input_proxy_pyvariable);
  }
  return proxy_trace_info_ptr();
}
 
//********************************************************************
proxy_trace_info_ptr get_view_proxy_info_for_rep(const proxy_trace_info_ptr rep_info)
{
  QList<proxy_trace_info_ptr> aTmpList;
  aTmpList.append(globals.registered_proxies);
  aTmpList.append(globals.last_registered_proxies);
  foreach (proxy_trace_info_ptr p, aTmpList) {
    // If the proxy is a view, check for rep_info.Proxy in the
    // view's 'Representation' property
    if (p->Group == "views") {
      vtkSMProxyProperty* rep_prop = 
        dynamic_cast<vtkSMProxyProperty*>(p->Proxy->GetProperty("Representations"));
      if (rep_prop) {
        for (int i = 0; i < rep_prop->GetNumberOfProxies(); i++) {
          if (rep_info->Proxy == rep_prop->GetProxy(i)) 
            return p;
        }
      }
    }
  }
  return proxy_trace_info_ptr();
}

//********************************************************************
QString get_source_proxy_registration_name(vtkSMProxy* proxy)
{
  return vtkSMObject::GetProxyManager()->GetProxyName("sources", proxy);
}

//********************************************************************
QString get_view_proxy_registration_name(vtkSMProxy* proxy)
{
  return vtkSMObject::GetProxyManager()->GetProxyName("views", proxy);
}

//********************************************************************
QString get_representation_proxy_registration_name(vtkSMProxy* proxy)
{
  return vtkSMObject::GetProxyManager()->GetProxyName("representations", proxy);
}

//********************************************************************
QString make_comma_separated_string(QStringList values)
{
  QString ret=values.join(", ");
  if(ret!="")
    ret = QString(" %1 ").arg(ret);
  return ret;
}

QStringList wrap_property(vtkSMProxy* proxy, vtkSMProperty* smproperty)
{
  QStringList propertyList;
  if(smproperty->IsA("vtkSMVectorProperty")) {
    vtkSMVectorProperty* property = dynamic_cast<vtkSMVectorProperty*>(smproperty);
    int nElem = property->GetNumberOfElements();
    int di = 1;
    vtkSMDomain* dom = NULL;
    if(property->IsA("vtkSMStringVectorProperty")) {
      dom = property->GetDomain("array_list");
      if(dom!=NULL && dom->IsA("vtkSMArraySelectionDomain") && property->GetRepeatable()) {
	if(nElem%2 != 0) {
	  cerr << "!!!Error: The SMProperty with XML label" << smproperty->GetXMLLabel() << "has a size that is not a multiple of 2." << endl;
	  nElem=0;
	} else {
	  di=2;
	}
      }
    } else if(property->IsA("vtkSMIntVectorProperty")) {
      dom = property->GetDomain("enum");
    }
    for(int i = 0; i < nElem; i+=di) {
      bool ap=true;
      if(di!=1) {
	if(dynamic_cast<vtkSMStringVectorProperty*>(property)->GetElement(i+1) == "0") {
	  ap=false;
	}
      }
      if(ap) {
	if(property->IsA("vtkSMStringVectorProperty"))
	  propertyList.append(QString("'%1'").arg(dynamic_cast<vtkSMStringVectorProperty*>(property)->GetElement(i)));
	if(property->IsA("vtkSMIntVectorProperty")) {
	  if(dom == NULL) {
	    propertyList.append(QString("%1").arg(dynamic_cast<vtkSMIntVectorProperty*>(property)->GetElement(i)));
	  } else {
	    for(int j = 0; j < dynamic_cast<vtkSMEnumerationDomain*>(dom)->GetNumberOfEntries(); j++) {
	      if(dynamic_cast<vtkSMEnumerationDomain*>(dom)->GetEntryValue(j) == dynamic_cast<vtkSMIntVectorProperty*>(property)->GetElement(i)) {
		propertyList.append(QString("'%1'").arg(dynamic_cast<vtkSMEnumerationDomain*>(dom)->GetEntryText(j)));
		break;
	      }
	    }
	  }
	}
	if(property->IsA("vtkSMIdTypeVectorProperty"))
	  propertyList.append(QString("%1").arg(dynamic_cast<vtkSMIdTypeVectorProperty*>(property)->GetElement(i)));
	if(property->IsA("vtkSMDoubleVectorProperty"))
	  propertyList.append(QString("%1").arg(dynamic_cast<vtkSMDoubleVectorProperty*>(property)->GetElement(i)));
      }
    }
  }
  return propertyList;
}

//********************************************************************
QString vector_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                   const prop_trace_info_ptr propInfo)
{
  vtkSMProxy* proxy = proxyInfo->Proxy;
  vtkSMProperty* prop = propInfo->Prop;
  QStringList propList = wrap_property(proxy, prop);
  QString pythonProp;
  if (propList.size() == 0) 
    pythonProp = "[]";
  if (propList.size() == 1) 
    return propList.first();
  if (propList.size() > 1) {
    pythonProp = make_comma_separated_string(propList);
    pythonProp = QString("[%1]").arg(pythonProp);
  }
  return pythonProp;
}

//********************************************************************
QString get_property_value_from_list_domain(const proxy_trace_info_ptr proxyInfo, 
                                            const prop_trace_info_ptr propInfo)
{
  vtkSMProxy* proxy = proxyInfo->Proxy;
  vtkSMProxyProperty* prop = dynamic_cast<vtkSMProxyProperty*>(propInfo->Prop);
  vtkSMProxyListDomain* listdomain = dynamic_cast<vtkSMProxyListDomain*>(prop->GetDomain("proxy_list"));
  if (listdomain != NULL && prop->GetNumberOfProxies() == 1) {
    vtkSMProxy* proxyPropertyValue = prop->GetProxy(0);   
    for (int i = 0; i < listdomain->GetNumberOfProxies(); i++) {
      if (listdomain->GetProxy(i) == proxyPropertyValue) {
	proxy_trace_info_ptr info = get_proxy_info(proxyPropertyValue);
	if (!info) {
	  proxy_trace_info_ptr infotmp(new proxy_trace_info(proxyPropertyValue, "helpers", listdomain->GetProxy(i)->GetXMLLabel()));
	  info = infotmp;
	}
	info->PyVariable = QString("%1.%2").arg(proxyInfo->PyVariable).arg(propInfo->PyVariable);
	globals.registered_proxies.append(info);
	return QString("\"%1\"").arg(listdomain->GetProxy(i)->GetXMLLabel());
      }
    }
  }
  return QString();
}

//********************************************************************
QString proxy_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                  const prop_trace_info_ptr propInfo)
{
  QString strValue = get_property_value_from_list_domain(proxyInfo, propInfo);
  if (!strValue.isNull()) 
    return strValue;
  vtkSMProxy* proxy = proxyInfo->Proxy;
  vtkSMProxyProperty* prop = dynamic_cast<vtkSMProxyProperty*>(propInfo->Prop);
  // Create a list of the python variables for each proxy in the property vector
  QStringList nameList;
  for (int i = 0; i < prop->GetNumberOfProxies(); i++) {
    vtkSMProxy* inputProxy = prop->GetProxy(i);
    proxy_trace_info_ptr info = get_proxy_info(inputProxy);
    if (info && (!info->PyVariable.isNull())) 
      nameList.append(info->PyVariable);
  }
  if (nameList.size() == 0) 
    return "[]";
  if (nameList.size() == 1) 
    return nameList.first();
  if (nameList.size() > 1) {
    QString nameListStr = make_comma_separated_string(nameList);
    return QString("[%1]").arg(nameListStr);
  }
}

//********************************************************************
QString input_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                  const prop_trace_info_ptr propInfo)
{
  return proxy_smproperty_tostring(proxyInfo, propInfo);
}

//********************************************************************
void trace_proxy_rename(const proxy_trace_info_ptr proxy_info, const char* new_name)
{
  if ((!proxy_info) || (proxy_info->Group != "sources"))
    return;
  QString old_pyvariable = proxy_info->PyVariable;
  proxy_info->PyVariable = pyvariable_from_proxy_name(new_name);
  proxy_info->ignore_next_unregister = true;
  QString newName(new_name);
  QString name_to_set = newName.replace("\"", "");
  globals.trace_output.append(QString("RenameSource(\"%1\", %2)").arg(name_to_set).arg(old_pyvariable));
  globals.trace_output.append(QString("%1 = %2").arg(proxy_info->PyVariable).arg(old_pyvariable));
  globals.trace_output.append(QString("del %1").arg(old_pyvariable));
}

//********************************************************************
proxy_trace_info_ptr trace_proxy_registered(vtkSMProxy* proxy, 
                                         const QString& proxyGroup, 
                                         const char* proxyName)
{
  //if trace_globals.verbose:
  //  print "Proxy '%s' registered in group '%s'" % (proxyName, proxyGroup)
  if (!globals.traced_proxy_groups.contains(proxyGroup))
    return proxy_trace_info_ptr();

  proxy_trace_info_ptr info(new proxy_trace_info(proxy, proxyGroup, proxyName));
  globals.last_registered_proxies.append(info);
  if (globals.capture_all_properties) {
    vtkSMPropertyIterator* itr = proxy->NewPropertyIterator();
    for (itr->Begin(); !itr->IsAtEnd(); itr->Next()) { 
      vtkSMProperty* prop = itr->GetProperty();
      if (prop->GetInformationOnly() || prop->GetIsInternal()) continue;
      trace_property_modified(info, prop);
    }
  }
  return info;
}

//********************************************************************
prop_trace_info_ptr trace_property_modified(proxy_trace_info_ptr info, vtkSMProperty* prop)
{
  prop_trace_info_ptr propInfo(new prop_trace_info(info, prop));
  //if (globals.verbose)
  //  print "Property '%s' modifed on proxy '%s'" % (propInfo.PyVariable, info.ProxyName)
  QString propValue;
  if (prop->IsA("vtkSMVectorProperty"))
    propValue = vector_smproperty_tostring(info, propInfo);
  else if (prop->IsA("vtkSMInputProperty"))
    propValue = input_smproperty_tostring(info, propInfo);
  else if (prop->IsA("vtkSMProxyProperty"))
    propValue = proxy_smproperty_tostring(info, propInfo);
  if (!propValue.isNull()) {
    info->Props[prop] = propValue;
    info->ModifiedProps[propInfo->PyVariable] = propValue;
    info->CurrentProps[propInfo->PyVariable] = propValue;
  }
  return propInfo;
}

//********************************************************************
QList<proxy_trace_info_ptr> sort_proxy_info_by_group(const QList<proxy_trace_info_ptr>& infoList)
{
  QList<proxy_trace_info_ptr> views;
  QList<proxy_trace_info_ptr> sources;
  QList<proxy_trace_info_ptr> representations;
  QList<proxy_trace_info_ptr> other;
  foreach (proxy_trace_info_ptr i, infoList) {
    if (i->Group == "views") views.append(i);
    else if (i->Group == "sources") sources.append(i);
    else if (i->Group == "representations") representations.append(i);
    else other.append(i);
  }
  views.append(sources);
  views.append(other);
  views.append(representations);  
  return views;
}

//********************************************************************
void append_trace()
{
  // Get the list of last registered proxies in sorted order
  QList<proxy_trace_info_ptr> modified_proxies = 
    sort_proxy_info_by_group(globals.last_registered_proxies);

  // Now append the existing proxies to the list
  foreach (proxy_trace_info_ptr p, globals.registered_proxies)
    modified_proxies.append(p);

  foreach(proxy_trace_info_ptr info, modified_proxies) {
    QString traceOutput = "";

    // Generate list of tuples : (propName, propValue)
    QMap<QString, QString> propNameValues;
    QMap<QString, QString>::const_iterator aIt;
    for (aIt = info->ModifiedProps.constBegin(); aIt!=info->ModifiedProps.constEnd(); ++aIt) {
      QString propName = aIt.key();
      QString propValue = aIt.value();
      if (propIsIgnored(info, propName))
        continue;

      // Note, the 'Input' property is ignored for representations, so we are
      // only dealing with filter proxies here.  If the 'Input' property is a
      // single value (not a multi-input filter), then ensure the input is
      // the active source and leave the 'Input' property out of the
      // propNameValues list.
      if ((propName == "Input") && (!propValue.contains("["))) {
        proxy_trace_info_ptr inputProxyInfo = get_proxy_info(propValue);
        ensure_active_source(inputProxyInfo);
        continue;
      }
      propNameValues[propName] = propValue;
    }

    // Clear the modified prop list
    info->ModifiedProps.clear();

    // If info is in the last_registered_proxies list, then we need to add the
    // proxy's constructor call to the trace
    if(globals.last_registered_proxies.contains(info)) {
      // Determine the function call to construct the proxy
      bool setPropertiesInCtor = true;
      QStringList ctorArgs;
      QString extraCtorCommands = "";
      QString ctorMethod = make_name_valid(info->Proxy->GetXMLLabel());
      if (info->Group == "sources") {
        // track it as the last active source now
        globals.last_active_source = info->Proxy;
        // maybe append the guiName property
        if (globals.use_gui_name)
          ctorArgs.append(QString("guiName=\"%1\"").arg(info->ProxyName));
      }
      if (info->Group == "representations") {
        ctorMethod = "Show";
        setPropertiesInCtor = false;
        // Ensure the input proxy is the active source:
        proxy_trace_info_ptr input_proxy_info = get_input_proxy_info_for_rep(info);
        if (input_proxy_info)
          ensure_active_source(input_proxy_info);
        
        // Ensure the view is the active view:
        proxy_trace_info_ptr view_proxy_info = get_view_proxy_info_for_rep(info);
        if (view_proxy_info) {
          ensure_active_view(view_proxy_info);
        }
      }
      if (info->Group == "scalar_bars") {
        ctorMethod = "CreateScalarBar";
        extraCtorCommands = 
          QString("GetRenderView().Representations.append(%1)").arg(info->PyVariable);
      }
      if (info->Group == "views") {
        ctorMethod = "CreateRenderView";
        // Now track it as the last active view
        globals.last_active_view = info->Proxy;
        setPropertiesInCtor = false;
      }
      if (info->Group == "lookup_tables")
        ctorMethod = "CreateLookupTable";


      if (setPropertiesInCtor) {
        QMap<QString, QString>::const_iterator aIt1;
        for (aIt1 = propNameValues.constBegin(); aIt1!=propNameValues.constEnd(); ++aIt1) {
          QString propName = aIt1.key();
          QString propValue = aIt1.value();
          ctorArgs.append(QString("%1=%2").arg(propName).arg(propValue));
        }
        propNameValues.clear();
      }
      QString ctorArgString = make_comma_separated_string(ctorArgs);
      traceOutput = 
        QString("%1 = %2(%3)\n%4").arg(info->PyVariable).arg(ctorMethod).arg(ctorArgString).arg(extraCtorCommands);
    }
    // Set properties on the proxy

    QMap<QString, QString>::const_iterator aIt2;
    for (aIt2 = propNameValues.constBegin(); aIt2!=propNameValues.constEnd(); ++aIt2) {
      QString propName = aIt2.key();
      QString propValue = aIt2.value();
      traceOutput += QString("%1.%2 = %3\n").arg(info->PyVariable).arg(propName).arg(propValue);
    }
    if (traceOutput.length() > 0){
      globals.trace_output.append(traceOutput);
    }
  }
  foreach (proxy_trace_info_ptr p, globals.last_registered_proxies) {
    globals.registered_proxies.append(p);
  }
  while (globals.last_registered_proxies.size() > 0)
    globals.last_registered_proxies.removeLast();
}

//********************************************************************
QString get_trace_string()
{
  append_trace();
  QString s = globals.trace_output.join("\n");
  s += globals.trace_output_endblock + "\n";

  return s;
}

//********************************************************************
void save_trace(const QString& fileName)
{
  QFile file(fileName);
  if(!file.open(QFile::WriteOnly))
    return;

  QString trace = get_trace_string();
  QTextStream out(&file);
  out << trace;
  file.close();
}

//********************************************************************
//QString print_trace()

//********************************************************************
void on_proxy_registered(vtkObject* obj, unsigned long , void* , void *)
{
  vtkSMPythonTraceObserver* o = dynamic_cast<vtkSMPythonTraceObserver*>(obj);
  if (!o) return;

  vtkSMProxy* p = o->GetLastProxyRegistered();
  const char* pGroup = o->GetLastProxyRegisteredGroup();
  const char* pName = o->GetLastProxyRegisteredName();
  if (p && pGroup && pName) {
    proxy_trace_info_ptr proxy_info = get_proxy_info(p, false);
    // handle source proxy rename, no-op if proxy isn't a source
    if (proxy_info) 
      trace_proxy_rename(proxy_info, pName);
    else
      trace_proxy_registered(p, pGroup, pName);
  }
}

//********************************************************************
void on_proxy_unregistered(vtkObject* obj, unsigned long , void* , void*)
{
  vtkSMPythonTraceObserver* o = dynamic_cast<vtkSMPythonTraceObserver*>(obj);
  if (!o) return;

  vtkSMProxy* p = o->GetLastProxyUnRegistered();
  const char* pGroup = o->GetLastProxyUnRegisteredGroup();
  const char* pName = o->GetLastProxyUnRegisteredName();
  if (p && pGroup && pName) {
    proxy_trace_info_ptr proxy_info = get_proxy_info(p);
    if (proxy_info != NULL && proxy_info->Proxy != NULL) {
      if (proxy_info->ignore_next_unregister) {
        proxy_info->ignore_next_unregister = false;
        return;
      }
      globals.trace_output.append(QString("Delete(%1)").arg(proxy_info->PyVariable));
      if (globals.last_registered_proxies.contains(proxy_info)) {
        int aInd = globals.last_registered_proxies.indexOf(proxy_info);
        globals.last_registered_proxies.removeAt(aInd);
      }
      if (globals.registered_proxies.contains(proxy_info)) {
        int aInd = globals.registered_proxies.indexOf(proxy_info);
        globals.registered_proxies.removeAt(aInd);
      }
    }
  }
}

//********************************************************************
void on_property_modified(vtkObject* obj, unsigned long , void* , void*)
{
  vtkSMPythonTraceObserver* o = dynamic_cast<vtkSMPythonTraceObserver*>(obj);
  if (!o) return;

  const char* propName = o->GetLastPropertyModifiedName();
  vtkSMProxy* proxy = o->GetLastPropertyModifiedProxy();
  if ((propName != NULL) && (proxy != NULL)) {
    vtkSMProperty* prop = proxy->GetProperty(propName);
    if (prop->GetInformationOnly() || prop->GetIsInternal()) return;

    // small hack here: some view properties are modified before the view
    // is registered.  We don't want to call get_proxy_info until after
    // the view is registered, so for now lets ignore these properties:
    QStringList ignoredViewProp = ignoredViewProperties();
    if (ignoredViewProp.contains(propName)) return;

    proxy_trace_info_ptr info = get_proxy_info(proxy);
    if ((info != NULL && info->Proxy != NULL) && (prop != NULL))
      trace_property_modified(info, prop);
  }
}

//********************************************************************
void on_update_information(vtkObject*, unsigned long , void* , void*)
{
  append_trace();
}

//********************************************************************
void add_observers()
{
  vtkCallbackCommand *aCommand = NULL;
  vtkSMPythonTraceObserver* o = trace_observer();
  aCommand = vtkCallbackCommand::New();
  aCommand->SetCallback(on_proxy_registered);
  o->AddObserver("RegisterEvent", aCommand);
  aCommand->Delete();
  aCommand = vtkCallbackCommand::New();
  aCommand->SetCallback(on_proxy_unregistered);
  o->AddObserver("UnRegisterEvent", aCommand);
  aCommand->Delete();
  aCommand = vtkCallbackCommand::New();
  aCommand->SetCallback(on_property_modified);
  o->AddObserver("PropertyModifiedEvent", aCommand);
  aCommand->Delete();
  aCommand = vtkCallbackCommand::New();
  aCommand->SetCallback(on_update_information);
  o->AddObserver("UpdateInformationEvent", aCommand);
  aCommand->Delete();
  globals.observer_active = true;
}

//********************************************************************
void reset_trace_observer()
{
  if (myInitialised) 
    globals.observer->Delete();
  globals.observer = vtkSMPythonTraceObserver::New();
  globals.observer_active = false;
  myInitialised = true;
}

//********************************************************************
void reset_trace_globals()
{
  globals.capture_all_properties = false;
  globals.use_gui_name = false;
  globals.verbose = false;
  globals.active_source_at_start = 0;
  globals.active_view_at_start = 0;
  globals.last_active_view = 0;
  globals.last_active_source = 0;
  globals.last_registered_proxies.clear();
  globals.registered_proxies.clear();
  globals.trace_output.clear();
  globals.trace_output << "try: pvsimple\nexcept: from pvsimple import *\n";
  globals.trace_output_endblock = "Render()";
  globals.traced_proxy_groups.clear();
  globals.traced_proxy_groups << "sources"<<"representations"<<"views"<<"lookup_tables"<<"scalar_bars";
  globals.ignored_view_properties.clear();
  globals.ignored_view_properties << "ViewSize"<<"GUISize"<<"ViewPosition"<<"ViewTime"<<"Representations";
  globals.ignored_representation_properties.clear();
  globals.ignored_representation_properties << "Input";
  reset_trace_observer();
}

//********************************************************************
void clear_trace()
{
  reset_trace_globals();
}

//********************************************************************
void stop_trace()
{
  reset_trace_observer();
}

//********************************************************************
void start_trace(bool CaptureAllProperties, bool UseGuiName, bool Verbose)
{
  clear_trace();
  add_observers();
  // VSV: Has to be defined
  //globals.active_source_at_start = simple.GetActiveSource()
  //globals.active_view_at_start = simple.GetActiveView()
  //globals.capture_all_properties = CaptureAllProperties
  //globals.use_gui_name = UseGuiName
  //globals.verbose = Verbose
}
