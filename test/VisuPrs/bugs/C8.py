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

# This case corresponds to: /visu/bugs2/C8 case

from paravistest import tablesdir
from presentations import *
import pvserver as paravis
import pvsimple

# 1. Import table from file
file_path = tablesdir + "tables_test.xls"

print 'Import tables_test.xls ....',
file_path = tablesdir + "tables_test.xls"
table_reader = pvsimple.TableReader(FileName=file_path)
table_reader.UpdatePipeline()
if table_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Show curves
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)

xy_view = pvsimple.CreateXYPlotView()
xy_view.ChartTitle = "The viewer for Curves from the Table"


print 'Get available tables .....'
available_tables = table_reader.GetPropertyValue("AvailableTables")
if (available_tables is None) or (len(available_tables) == 0):
    print "FAILED"
else:
    print available_tables

for table in available_tables:
    table_reader.TableNumber = available_tables.GetData().index(table)

    xy_rep = pvsimple.Show()
    xy_rep.AttributeType = 'Row Data'
    xy_rep.UseIndexForXAxis = 0
    xy_rep.SeriesVisibility = ['0', '0']
    
    pvsimple.Render()


    
