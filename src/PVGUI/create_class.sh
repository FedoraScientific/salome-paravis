#!/bin/bash

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
