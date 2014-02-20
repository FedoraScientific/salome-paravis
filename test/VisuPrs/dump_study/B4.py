# Copyright (C) 2010-2014  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# This case corresponds to: /visu/dump_study/B4 case

import paravistest
from presentations import *
import paravis
from pvsimple import *

my_paravis = paravis.myParavis

# 1. Table creation
title = "My Table"
errors = 0

table_title = "My Table"

# define script for table creation
table_script = """
import math


# Get table output
table = self.GetTableOutput()

# Create first column
col1 = vtk.vtkIntArray()
col1.SetName('First Column')
col1.InsertNextValue(1)
col1.InsertNextValue(4)
table.AddColumn(col1)

# Create second column
col2 = vtk.vtkDoubleArray()
col2.SetName('Second Column')
col2.InsertNextValue(2)
col2.InsertNextValue(5)
table.AddColumn(col2)

# Create third column
col3 = vtk.vtkDoubleArray()
col3.SetName('Third Column')
col3.InsertNextValue(3)
col3.InsertNextValue(6)
table.AddColumn(col3)
"""

# creating programmable source for the table
table = ProgrammableSource()
table.OutputDataSetType = 'vtkTable'
table.Script = table_script
RenameSource(title, table)
table.UpdatePipeline()

orig_script = table.Script

# 2. Dump Study
path_to_save = os.path.join(os.getenv("HOME"), "table.py")
SaveTrace(path_to_save)

# 3. Delete the table
Delete(table)

# 4. Execution of the created script
execfile(path_to_save)

# 5. Check the restored table
table = FindSource(title)
if table is None:
    raise RuntimeError, "There is no table in the study (must be created by executed python script)!!!"

if table.Script != orig_script:
    print "ERROR!!! The script value is incorrect:"
    print table.Script
    errors += 1
    
if errors > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
