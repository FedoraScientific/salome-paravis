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

# This case corresponds to: /visu/2D_viewer/A0 case

from paravistest import *
from presentations import *
from pvsimple import *

# Create view
aXYPlot = CreateXYPlotView()
Render(aXYPlot)

# Set title
print "Default chart title  ...", aXYPlot.ChartTitle
aXYPlot.ChartTitle = 'title of XY plot'
print "New chart title      ...", aXYPlot.ChartTitle

# Apply custom axes range
aXYPlot.LeftAxisUseCustomRange = 1
aXYPlot.BottomAxisUseCustomRange = 1

# Show grids
aXYPlot.ShowLeftAxisGrid = 1
aXYPlot.ShowBottomAxisGrid = 1

aMinX = 0
aMaxX = 100
aMinY = 0
aMaxY = 10
aXYPlot.LeftAxisRangeMinimum = aMinY
aXYPlot.LeftAxisRangeMaximum = aMaxY
aXYPlot.BottomAxisRangeMinimum = aMinX
aXYPlot.BottomAxisRangeMaximum = aMaxX

# Set horizontal scaling for left and bottom axis
print "Default logarithmic scaling ... left = %s, bottom = %s" % ( aXYPlot.LeftAxisLogScale, aXYPlot.BottomAxisLogScale )
# Set logarithmic scaling
aXYPlot.LeftAxisLogScale = 1
aXYPlot.BottomAxisLogScale = 1
print "Set logarithmic scaling     ... left = %s, bottom = %s" % ( aXYPlot.LeftAxisLogScale, aXYPlot.BottomAxisLogScale )
# Set linear scaling
aXYPlot.LeftAxisLogScale = 0
aXYPlot.BottomAxisLogScale = 0
print "Set linear scaling          ... left = %s, bottom = %s" % ( aXYPlot.LeftAxisLogScale, aXYPlot.BottomAxisLogScale )

# Axis titles
print "Default title of the left axis    ...", aXYPlot.LeftAxisTitle
aXYPlot.LeftAxisTitle = "Ytitle of XY plot"
print "New title of the left axis        ...", aXYPlot.LeftAxisTitle
print "Default title of the bottom axis  ...", aXYPlot.BottomAxisTitle
aXYPlot.BottomAxisTitle = "Xtitle of XY plot"
print "New title of the bottom axis      ...", aXYPlot.BottomAxisTitle

# Show/hide legend
aXYPlot.ShowLegend = 1
aXYPlot.ShowLegend = 0
