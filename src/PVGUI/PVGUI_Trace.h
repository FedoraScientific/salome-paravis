//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PVGUI_Trace.h
//  Author : Vitaly Smetannikov
//  Module : PARAVIS
//
#ifndef __PARAVIS_TRACE_I_H__
#define __PARAVIS_TRACE_I_H__

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <boost/shared_ptr.hpp>

class vtkSMPythonTraceObserver;
class vtkSMProxy;
class vtkSMProperty;
class vtkObject;

class proxy_trace_info: public QObject
{
  Q_OBJECT
public:
  proxy_trace_info(vtkSMProxy* proxy, const QString& proxyGroup, const QString& proxyName);

  vtkSMProxy*            Proxy;
  QString                Group;
  QString                ProxyName;
  QString                PyVariable;
  QMap<vtkSMProperty*, QString> Props;
  QMap<QString, QString> CurrentProps;
  QMap<QString, QString> ModifiedProps;
  bool                   ignore_next_unregister;
};

typedef boost::shared_ptr<proxy_trace_info> proxy_trace_info_ptr;



class prop_trace_info: public QObject
{
  Q_OBJECT
public:
  prop_trace_info(proxy_trace_info_ptr proxyTraceInfo, vtkSMProperty* prop);

  vtkSMProperty* Prop;
  QString        PyVariable;
};

typedef boost::shared_ptr<prop_trace_info>  prop_trace_info_ptr;

struct trace_globals
{
  vtkSMPythonTraceObserver* observer;
  bool observer_active;
  bool capture_all_properties;
  bool use_gui_name;
  bool verbose;

  vtkSMProxy* active_source_at_start;
  vtkSMProxy* last_active_source;

  vtkSMProxy*   active_view_at_start;
  vtkSMProxy*   last_active_view;

  QList<proxy_trace_info_ptr> last_registered_proxies;
  QList<proxy_trace_info_ptr> registered_proxies;

  QStringList        trace_output;
  QString            trace_output_endblock;
  QStringList        traced_proxy_groups;
  QStringList        ignored_view_properties;
  QStringList        ignored_representation_properties;
};


QString make_name_valid(const QString& theName);

void reset_trace_observer();

void reset_trace_globals();

QString pyvariable_from_proxy_name(const QString& proxy_name);

vtkSMPythonTraceObserver* trace_observer();

QStringList ignoredViewProperties();

QStringList ignoredRepresentationProperties();

bool propIsIgnored(const proxy_trace_info_ptr info, const QString& propName);

void track_existing_sources();

proxy_trace_info_ptr track_existing_source_proxy(vtkSMProxy* proxy, const QString& proxy_name);

proxy_trace_info_ptr track_existing_view_proxy(vtkSMProxy* proxy, const QString& proxy_name);

proxy_trace_info_ptr track_existing_representation_proxy(vtkSMProxy* proxy, const QString& proxy_name);

proxy_trace_info_ptr track_existing_proxy(vtkSMProxy* proxy);

proxy_trace_info_ptr get_proxy_info(const QString& p, bool search_existing=true);
proxy_trace_info_ptr get_proxy_info(vtkSMProxy* p, bool search_existing=true);

void ensure_active_source(const proxy_trace_info_ptr proxy_info);
void ensure_active_view(const proxy_trace_info_ptr proxy_info);

proxy_trace_info_ptr get_input_proxy_info_for_rep(const proxy_trace_info_ptr rep_info);
proxy_trace_info_ptr get_view_proxy_info_for_rep(const proxy_trace_info_ptr rep_info);

QString get_source_proxy_registration_name(vtkSMProxy* proxy);

QString get_view_proxy_registration_name(vtkSMProxy* proxy);

QString get_representation_proxy_registration_name(vtkSMProxy* proxy);

QString make_comma_separated_string(QStringList values);

QString vector_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                   const prop_trace_info_ptr propInfo);

QString get_property_value_from_list_domain(const proxy_trace_info_ptr proxyInfo, 
                                            const prop_trace_info_ptr propInfo);

QString proxy_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                  const prop_trace_info_ptr propInfo);

QString input_smproperty_tostring(const proxy_trace_info_ptr proxyInfo, 
                                  const prop_trace_info_ptr propInfo);

void trace_proxy_rename(const proxy_trace_info_ptr proxy_info, const char* new_name);

proxy_trace_info_ptr trace_proxy_registered(vtkSMProxy* proxy, 
                                         const QString& proxyGroup, 
                                         const char* proxyName);

prop_trace_info_ptr trace_property_modified(proxy_trace_info_ptr info, vtkSMProperty* prop);


QList<proxy_trace_info_ptr> sort_proxy_info_by_group(const QList<proxy_trace_info_ptr>& infoList);

void append_trace();

QString get_trace_string();

void save_trace(const QString& fileName);

//QString print_trace();

void on_proxy_registered(vtkObject* o, unsigned long eid, void* clientdata, void *calldata);

void on_proxy_unregistered(vtkObject* o, unsigned long eid, void* clientdata, void *calldata);

void on_property_modified(vtkObject* o, unsigned long eid, void* clientdata, void *calldata);

void on_update_information(vtkObject* o, unsigned long eid, void* clientdata, void *calldata);

void add_observers();

void clear_trace();

void stop_trace();

void start_trace(bool CaptureAllProperties=false, bool UseGuiName=false, bool Verbose=false);


#endif
