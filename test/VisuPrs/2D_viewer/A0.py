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

# This case corresponds to: /visu/2D_viewer/A0 case

import paravistest

from pvsimple import CreateXYPlotView, Delete

# Create view
aXYPlot = CreateXYPlotView()
# Delete view
Delete(aXYPlot)
# Create view again
aXYPlot = CreateXYPlotView()

# Set title
print "Default chart title    ...", aXYPlot.ChartTitle
aXYPlot.ChartTitle = '   '
aXYPlot.ChartTitle = 'title of XY plot'
print "Chart title for xyplot ...", aXYPlot.ChartTitle

error = 0

aXYPlot.AxisUseCustomRange = [1, 1, 0, 0]

# Show grids
aXYPlot.ShowAxisGrid = [1, 1, 0, 0]

aMinX = 0
aMaxX = 100
aMinY = 0
aMaxY = 10
aXYPlot.BottomAxisRange = [aMinX, aMaxX]
aXYPlot.LeftAxisRange = [aMinY, aMaxY]

# Set horizontal scaling for left and bottom axis
print "Default logarithmic scaling  ...", aXYPlot.AxisLogScale
# Set logarithmic scaling
aXYPlot.AxisLogScale = [1, 1, 0, 0]
print "Logarithmic scaling          ...", aXYPlot.AxisLogScale
# Set linear scaling
aXYPlot.AxisLogScale = [0, 0, 0, 0]
print "Linear scaling               ...", aXYPlot.AxisLogScale

# Axis titles
print "Default title of the left axis    ...", aXYPlot.AxisTitle[0]
aXYPlot.AxisTitle[0] = '    '
aXYPlot.AxisTitle[0] ="Xtitle of XY plot"
print "Title of the left axis            ...", aXYPlot.AxisTitle[0]
print "Default title of the bottom axis  ...", aXYPlot.AxisTitle[1]
aXYPlot.AxisTitle[1] = '    '
aXYPlot.AxisTitle[1] = "Ytitle of XY plot"
print "Title of the bottom axis          ...", aXYPlot.AxisTitle[1]

# Show/hide legend
aXYPlot.ShowLegend = 1
aXYPlot.ShowLegend = 0

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
