#!/bin/bash
# Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

import sys
import os

ffile = "PARAVIS_CreateClass.cxx"
wfile = os.path.join(sys.argv[1], "wrapfiles.txt")

ffile_stream = open(ffile, "w")

ffile_stream.write('#include <iostream>' + '\n')
ffile_stream.write('#include <QString>' + '\n')
ffile_stream.write('#include "PARAVIS_Gen_i.hh"' + '\n')
wfile_stream = open(wfile)
for line in wfile_stream:
    ffile_stream.write('#include "PARAVIS_Gen_%s_i.hh"'%(line.split()[0]) + '\n')
    pass
wfile_stream.close()
ffile_stream.write('' + '\n')
ffile_stream.write('PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString& theClassName)' + '\n')
ffile_stream.write('{' + '\n')
wfile_stream = open(wfile)
lst = []
for line in wfile_stream:
    lst.append(line.split()[0])
    pass
wfile_stream.close()
lst.reverse()
for item in lst:
    ffile_stream.write('    if(theClassName == "%s" || (Inst != NULL && Inst->IsA("%s")))'%(item, item) + '\n')
    ffile_stream.write('      return new PARAVIS::%s_i();'%(item) + '\n')
    pass
ffile_stream.write('' + '\n')
ffile_stream.write('    cerr << "The class " << theClassName.toStdString() << " is not created!" << endl;' + '\n')
ffile_stream.write('    return new PARAVIS::PARAVIS_Base_i();' + '\n')
ffile_stream.write('}' + '\n')
ffile_stream.close()
