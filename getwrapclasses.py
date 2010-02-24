import paraview, os, sys
import string
import re

classeslistsm = []
classeslistvtk = []

if os.name == "posix":
    from libvtkPVServerCommonPython import *
    from libvtkPVServerManagerPython import *
else:
    from vtkPVServerCommonPython import *
    from vtkPVServerManagerPython import *

l1 = dir()
for a in l1:
    if a.startswith("vtk") and (len(a) > 3):
        classeslistsm.append(a)
        
from paraview.vtk import *
l2 = dir()

for a in l2:
    if (a not in l1) and a.startswith("vtk"):
        classeslistvtk.append(a)

dic=dict();
pv_classes_new=classeslistsm
while len(pv_classes_new):
    pv_classes_cur=pv_classes_new
    pv_classes_new=[]
    for c in pv_classes_cur:
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
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk:
                            pv_classes_new.append(cn)
                            continue

                    m=re.match('\s*struct\s+(vtk\w+)\s*;',line)
                    if m is not None:
                        cn=m.group(1)
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk:
                            pv_classes_new.append(cn)
                            continue

                    m=re.match('\s*#include\s+"(vtk\w+)\.h"',line)
                    if m is not None:
                        cn=m.group(1)
                        if cn not in dic.keys() and cn not in pv_classes_new and cn in classeslistvtk:
                            pv_classes_new.append(cn)
                            continue

                    cm=re.findall('public\s+(vtk\w+)',line)
                    if len(cm) == 0:
                        continue
                    for cn in cm:
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
