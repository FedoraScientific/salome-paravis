#!/bin/bash
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

file=PARAVIS_CreateClass.cxx
wfile=$1/wrapfiles.txt

echo '#include <iostream>' > $file
echo '#include <QString>' >> $file
echo '#include "PARAVIS_Gen_i.hh"' >> $file
awk '{print "#include \"PARAVIS_Gen_"$1"_i.hh\"";}' $wfile >> $file
echo '' >> $file
echo 'PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString& theClassName)' >> $file
echo '{' >> $file
awk 'BEGIN {lst="";} {lst=lst" "$1;} END{i=split(lst,arr);for(j=i;j>0;j--) {print "    if(theClassName == \""arr[j]"\" || (Inst != NULL && Inst->IsA(\""arr[j]"\")))";print "      return new PARAVIS::"arr[j]"_i();"}}' $wfile >> $file
echo '' >> $file
echo '    cerr << "The class " << theClassName.toStdString() << " is not created!" << endl;' >> $file
echo '    return new PARAVIS::PARAVIS_Base_i();' >> $file
echo '}' >> $file
