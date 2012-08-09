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

# This case corresponds to: /visu/SWIG_scripts/B6 case
# Create table of integer, create table of real, create curves

from paravistest import tablesdir
import paravis
import pvsimple


# Define script for table of integer creation
script_ti = """
def add_column(table, name, lst):
    col = vtk.vtkIntArray()
    col.SetName(name)
    for item in lst:
        col.InsertNextValue(item)
    table.AddColumn(col)

# Get table output
table = self.GetTableOutput()

# Add columns
lst = [1,2,3,4,5,6,7,8,9,10]
add_column(table, 'FR', lst)

lst = [110,120,130,140,150,160,170,180,190,200]
add_column(table, 'SR', lst)

lst = [-1,272,0,0,-642,10000,13,578,-578,99]
add_column(table, 'TR', lst)
"""

# Define script for table of real creation
script_tr = """
import math


# Get table output
table = self.GetTableOutput()

# Create first column
col1 = vtk.vtkDoubleArray()
col1.SetName('Row 0')
for i in xrange(0, 21):
    col1.InsertNextValue(i * 10 + 1)
table.AddColumn(col1)

# Create the rest columns
for i in range(1, 11):
    col = vtk.vtkDoubleArray()
    col.SetName('Row ' + str(i))

    # Fill the next column
    col.InsertNextValue(1)
    for j in range(1, 21):
        if j % 2 == 1:
            col.InsertNextValue(math.log10(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)
        else:
            col.InsertNextValue(math.sin(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)

    table.AddColumn(col)
"""

# Creating programmable source for the table of integer
ps_ti = pvsimple.ProgrammableSource()
ps_ti.OutputDataSetType = 'vtkTable'
ps_ti.Script = script_ti
pvsimple.RenameSource("TEST table of integer", ps_ti)
ps_ti.UpdatePipeline()

# Creating programmable source for the table of real
ps_tr = pvsimple.ProgrammableSource()
ps_tr.OutputDataSetType = 'vtkTable'
ps_tr.Script = script_tr
pvsimple.RenameSource("TEST table of real", ps_tr)
ps_tr.UpdatePipeline()

# Replace the current view with chart line one
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)

xy_view1 = pvsimple.CreateXYPlotView()
xy_view1.ChartTitle = 'TEST table of real'
xy_view1.AxisTitle = ['[ Wt ]', 'Row 0 [ Hz ]']

# Display curves for the table of real
tr_rep = pvsimple.Show(ps_tr)
tr_rep.AttributeType = 'Row Data'
tr_rep.UseIndexForXAxis = 0
tr_rep.XArrayName = 'Row 0'
tr_rep.SeriesVisibility = ['Row 0', '0', 'Row 4', '0', \
'Row 6', '0', 'Row 8', '0', 'Row 9', '0']

# Set 'Square' marker style for 'Row 5' line
tr_rep.SeriesMarkerStyle = ['Row 5', '3']
# Set 'Dash' line style for 'Row 5' line
tr_rep.SeriesLineStyle = ['Row 5', '2']
# Set color for 'Row 5' line
tr_rep.SeriesColor = ['Row 5', '0', '0.7', '0.3']
# Set 'Diamond' marker style for 'Row 10' line
tr_rep.SeriesMarkerStyle = ['Row 10', '5']
# Set 'Dot' line style for 'Row 10' line
tr_rep.SeriesLineStyle = ['Row 10', '3']
# Set color for 'Row 10' line
tr_rep.SeriesColor = ['Row 10', '0.2', '0.2', '0.9']

# Create another chart line view
xy_view2 = pvsimple.CreateXYPlotView()
xy_view2.ChartTitle = 'TEST table of integer'
xy_view2.AxisTitle = ['', 'FR [ m/h ]']

# Display curves for the table of integer
ti_rep = pvsimple.Show(ps_ti, xy_view2)
ti_rep.AttributeType = 'Row Data'
ti_rep.UseIndexForXAxis = 0
ti_rep.XArrayName = 'FR'
ti_rep.SeriesVisibility = ['SR', '0']

# Change legend label for "FR" line
ti_rep.SeriesLabel = ["FR", "FREQ"]

# Set 'Square' marker style for 'FR' line
ti_rep.SeriesMarkerStyle = ['FR', '3']
# Set 'Dash' line style for 'FR' line
ti_rep.SeriesLineStyle = ['FR', '2']
# Set color for 'FR' line
ti_rep.SeriesColor = ['FR', '0', '0.7', '0.3']


# Update views
pvsimple.Render(xy_view1)
pvsimple.Render(xy_view2)
