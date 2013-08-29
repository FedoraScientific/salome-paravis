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

# This case corresponds to: /visu/dump_study/B1 case

from paravistest import tablesdir
from presentations import *
import paravis
from pvsimple import *

my_paravis = paravis.myParavis

# Import table from file
xls_file = tablesdir + "tables_test.xls"
table_reader = TableReader(FileName=xls_file)
if table_reader is None:
    raise RuntimeError, "Table was not imported..."

table_reader.UpdatePipeline()
RenameSource("tables_test.xls", table_reader)

# 2. Dump Study
path_to_save = os.path.join(os.getenv("HOME"), "table.py")
SaveTrace(path_to_save)

# 3. Delete the created objects
Delete(table_reader)

# 4. Execution of the created script
execfile(path_to_save)

# 5. Check the restored table
restored_obj = FindSource("tables_test.xls")
if restored_obj is None:
    raise RuntimeError, "There is no tables_test.xls table in the study (must be created by executed python script)!!!"

available_tables = restored_obj.GetPropertyValue("AvailableTables")

tables = ["Table:0", "Table toto 1", "sinus"]
errors = 0
count_not=0

for name in tables:
    if name not in available_tables:
	count_not += 1
	print "ERROR!!! Table with ", name, " name was not found"
        errors += 1
    else:
        print "\"" + name + "\" table was found..."

if count_not > 0:
    print "ERROR!!! "+str(count_not)+" table(s) was(were) not found!!!"
    errors += 1   	    
if errors > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."

