import paraview, os
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

pvhome = os.environ.get('PVHOME')
pvversion = os.environ.get('PVVERSION')
wrap_dir="include/paraview-"+pvversion

dic=dict();
pv_classes_new=classeslistsm
while len(pv_classes_new):
    pv_classes_cur=pv_classes_new
    pv_classes_new=[]
    for c in pv_classes_cur:
        filename=pvhome+"/"+wrap_dir+"/"+c+".h"
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

awidl='WRAP_IDL = '
awsk='WRAP_SK_FILES = '
awhh='WRAP_IDL_I_HH = '
awcc='WRAP_IDL_I_CC = '

fprefix='PARAVIS_Gen_'
fidl='.idl'
fsk='SK.cc'
fhh='_i.hh'
fcc='_i.cc'

idl_am=open('idl/wrap.am','w')
idl_i_am=open('src/PVGUI/wrap.am','w')

wrap_h=open('idl/vtkWrapIDL.h','w')
wrap_h.write('const char* wrapped_classes[] = {\n')

wrap_cxx=open('src/PVGUI/PARAVIS_CreateClass.cxx','w')
wrap_cxx.write('#include <iostream>\n')
wrap_cxx.write('#include <QString>\n')
wrap_cxx.write('#include "PARAVIS_Gen_i.hh"\n')

for c in pv_classes_sort:
    widl=fprefix+c+fidl
    wsk=fprefix+c+fsk
    whh=fprefix+c+fhh
    wcc=fprefix+c+fcc
    awidl+=' ' + widl
    awsk+=' ' + wsk
    awhh+=' ' + whh
    awcc+=' ' + wcc

    idl_am.write(widl+': vtkWrapIDL')
    for cc in dic[c][1]:
        idl_am.write(' '+fprefix+cc+fidl)
    idl_am.write('\n')
    idl_am.write('\t@./vtkWrapIDL '+pvhome+'/'+wrap_dir+'/'+c+'.h $(top_srcdir)/idl/hints 0 $@\n')
    idl_am.write('\n')


    idl_i_am.write(whh+': vtkWrapIDL_HH')
    for cc in dic[c][1]:
        idl_i_am.write(' '+fprefix+cc+fhh)
    idl_i_am.write('\n')
    idl_i_am.write('\t@./vtkWrapIDL_HH '+pvhome+'/'+wrap_dir+'/'+c+'.h $(top_srcdir)/idl/hints 0 $@\n')
    idl_i_am.write('\n')

    idl_i_am.write(wcc+': vtkWrapIDL_CC $(WRAP_IDL_I_HH)')
    for cc in dic[c][1]:
        idl_i_am.write(' '+fprefix+cc+fcc)
    idl_i_am.write('\n')
    idl_i_am.write('\t@./vtkWrapIDL_CC '+pvhome+'/'+wrap_dir+'/'+c+'.h $(top_srcdir)/idl/hints 0 $@\n')
    idl_i_am.write('\n')

    wrap_h.write('"'+c+'",\n')

    wrap_cxx.write('#include "'+fprefix+c+fhh+'"\n')

idl_am.write(awidl+'\n')
idl_am.write('\n')
idl_am.write(awsk+'\n')

idl_i_am.write(awhh+'\n')
idl_i_am.write('\n')
idl_i_am.write(awcc+'\n')
idl_i_am.write('\n')
idl_i_am.write('BUILT_SOURCES = $(WRAP_IDL_I_HH) $(WRAP_IDL_I_CC)\n')

wrap_h.write('""\n};\n')

wrap_cxx.write('\n')
wrap_cxx.write('PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString& theClassName)\n')
wrap_cxx.write('{\n')
for i in range(len(pv_classes_sort)-1,-1,-1):
    c=pv_classes_sort[i]
    wrap_cxx.write('    if(theClassName == "'+c+'" || (Inst != NULL && Inst->IsA("'+c+'")))\n')
    wrap_cxx.write('      return new PARAVIS::'+c+'_i();\n')
wrap_cxx.write('\n')
wrap_cxx.write('    cerr << "The class " << theClassName.toStdString() << " is not created!" << endl;\n')
wrap_cxx.write('    return new PARAVIS::PARAVIS_Base_i();\n')
wrap_cxx.write('}\n')

idl_am.close()
idl_i_am.close()
wrap_h.close()
wrap_cxx.close()

