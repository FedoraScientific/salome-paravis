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

# This case corresponds to: /visu/SWIG_scripts/C3 case
# Create table and display curves

import paraview.simple as pv
import paraview.servermanager as sm


# Define script for table creation
script = """
import math


# Get table output
table = self.GetTableOutput()

nb_rows = 10
nb_cols = 200

# Create first column
col1 = vtk.vtkDoubleArray()
col1.SetName('Frequency')
for i in xrange(0, nb_rows):
    col1.InsertNextValue(i * 10 + 1)
table.AddColumn(col1)

# Create the rest columns
for i in xrange(1, nb_cols + 1):
   col = vtk.vtkDoubleArray()
   col.SetName('Power ' + str(i))

   # Fill the next column
   for j in xrange(0, nb_rows):
      if j % 2 == 1:
         col.InsertNextValue(math.log10(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)
      else:
         col.InsertNextValue(math.sin(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)

   table.AddColumn(col)
"""

# Creating programmable source (table)
ps = pv.ProgrammableSource()
ps.OutputDataSetType = 'vtkTable'
ps.Script = script
pv.RenameSource("Very useful data", ps)
ps.UpdatePipeline()

# Display table
# TODO: no possibility to create spreadsheet view

# Display curves
xy_view = pv.CreateXYPlotView()
xy_view.ChartTitle = 'Very useful data'
xy_view.AxisTitle = ['[ Wt ]', 'Frequency [ Hz ]']

xy_rep = pv.Show(ps)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'Frequency'
xy_rep.SeriesVisibility = ['Frequency', '0']
pv.Render(xy_view)

# Hide legend
xy_view.ShowLegend = 0
pv.Render(xy_view)

# Set logarithmic scaling for X axis
xy_view.AxisLogScale[1] = 1
pv.Render(xy_view)

# Set linear scaling for X axis
xy_view.AxisLogScale[1] = 0
pv.Render(xy_view)

# Set logarithmic scaling for X axis
xy_view.AxisLogScale[0] = 1
pv.Render(xy_view)

# Show legend
xy_view.ShowLegend = 1
pv.Render(xy_view)
