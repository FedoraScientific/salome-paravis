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

# This case corresponds to: /visu/imps/B2 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
print 'Import "ResOK_0000.med"...............',

file_path = datadir + "ResOK_0000.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "import failed"
else:
    print "OK"

# 2. Get some information on the MED file
fields_on_nodes = med_reader.PointArrays
print "Field names on NODE: ", fields_on_nodes
is_ok = len(fields_on_nodes) == 2 and ("temperature" in fields_on_nodes) and ("vitesse" in fields_on_nodes)
if not is_ok:
    raise RuntimeError,  "=> Error in PointArrays property"

fields_on_cells = med_reader.CellArrays
print "Field names on CELL: ", fields_on_cells
is_ok = len(fields_on_cells) == 1 and ("pression" in fields_on_cells)
if not is_ok:
    raise RuntimeError,  "=> Error in CellArrays property"

timestamps = med_reader.TimestepValues.GetData()
print "timestamps: ", timestamps
if timestamps != [17.030882013694594]:
    raise RuntimeError,  "=> Wrong TimestepValues property value"



