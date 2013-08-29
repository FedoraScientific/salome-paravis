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

# This case corresponds to: /visu/SWIG_scripts/C6 case
# Import a table from file and show it in 2D viewer

from paravistest import tablesdir
from presentations import *
import paravis
import pvsimple


# 1. Import tables from file
file_path = tablesdir + "tables_test.xls"
table_reader = pvsimple.TableReader(FileName=file_path)
if table_reader is None:
    print "FAILED to import tables from tables_test.xls file."

# 2. Create curves viewer
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)
xy_view = pvsimple.CreateXYPlotView()

# 3. Display curves in the viewer
table_reader.TableNumber = 1
xy_rep = pvsimple.Show(table_reader)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'toto 1 [s]'
xy_rep.SeriesVisibility = [xy_rep.XArrayName, '0']
xy_rep.SeriesVisibility = ['vtkOriginalIndices', '0']
pvsimple.Render(xy_view)

