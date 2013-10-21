# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import paraview, os, sys
import string
import re

classeslistsm = []
classeslistvtk = []

from vtkPVCommonPython import *
from vtkPVClientServerCoreCorePython import *
from vtkPVClientServerCoreDefaultPython import *
from vtkPVClientServerCoreRenderingPython import *
from vtkPVServerImplementationCorePython import *
from vtkPVServerImplementationDefaultPython import *
from vtkPVServerImplementationRenderingPython import *
from vtkPVServerManagerApplicationPython import *
from vtkPVServerManagerCorePython import *
from vtkPVServerManagerDefaultPython import *
from vtkPVServerManagerRenderingPython import *
try:
    from vtkPVVTKExtensionsCorePython import *
except:
    pass
try:
    from vtkPVVTKExtensionsDefaultPython import *
except:
    pass
try:
    from vtkPVVTKExtensionsRenderingPython import *
except:
    pass
try:
    from vtkPVVTKExtensionsWebGLExporterPython import *
except:
    pass

l1 = dir()
for a in l1:
    if a.startswith("vtk") and (len(a) > 3):
        classeslistsm.append(a)
        
from paraview.vtk import *
l2 = dir()

for a in l2:
    if (a not in l1) and a.startswith("vtk"):
        classeslistvtk.append(a)

dic=dict()

non_wrap_list = ["vtkVariant", "vtkTimeStamp"]

pv_classes_new=classeslistsm
while len(pv_classes_new):
    pv_classes_cur=pv_classes_new
    pv_classes_new=[]
    for c in pv_classes_cur:
        ## ignore non wrappable classes
        if c in non_wrap_list:
            continue
        filename=sys.argv[1]+"/"+c+".h"
        if os.path.isfile(filename):
            c_new=[]
            f=open(filename)
            for line in f:
                if re.match('\s*/',line) is None:
                    if re.match('\s*public\s*:',line):
                        continue

                    m=re.match('\s*class\s+(vtk\w+)\s*;',line)
                    if m is not None:
                        cn=m.group(1)
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk and issubclass(globals()[cn], vtkObjectBase):
                            pv_classes_new.append(cn)
                            continue

                    m=re.match('\s*struct\s+(vtk\w+)\s*;',line)
                    if m is not None:
                        cn=m.group(1)
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk and issubclass(globals()[cn], vtkObjectBase):
                            pv_classes_new.append(cn)
                            continue

                    m=re.match('\s*#include\s+"(vtk\w+)\.h"',line)
                    if m is not None:
                        cn=m.group(1)
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk and issubclass(globals()[cn], vtkObjectBase):
                            pv_classes_new.append(cn)
                            continue

                    cm=re.findall('public\s+(vtk\w+)',line)
                    if len(cm) == 0:
                        continue
                    for cn in cm:
                        ## do not extract Call Back classes
                        if cn.count('vtkClassMemberCallback'):
                            continue
                        if cn not in dic.keys() and cn not in pv_classes_new:
                            pv_classes_new.append(cn)
                        if cn not in c_new:
                            c_new.append(cn)
            f.close()
            dic[c]=[0, c_new]


pv_classes_sort=[]

def collect_dic(cc):
    ret=[]
    for c in cc:
        if c not in dic.keys():
            continue
        if len(dic[c][1]) and dic[c][0] == 0:
             ret+=collect_dic(dic[c][1])
        if dic[c][0] == 0:
            ret.append(c)
            dic[c][0]=1
    return ret

pv_classes_sort=collect_dic(dic.keys())

wf_str=""
if(os.path.exists('wrapfiles.txt')):
    wf_txt=open('wrapfiles.txt','r')
    strs=wf_txt.readlines()
    wf_txt.close()
    for s in strs:
        wf_str+=s
str=""

for c in pv_classes_sort:
    str+=c
    for cc in dic[c][1]:
        str+=' '+cc
    str+='\n'

if(str!=wf_str):
    wf_txt=open('wrapfiles.txt','w')
    wf_txt.write(str)
    wf_txt.close()
