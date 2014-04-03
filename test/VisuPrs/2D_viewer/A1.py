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

# This case corresponds to: /visu/2D_viewer/A1 case

from paravistest import test_values
from pvsimple import CreateXYPlotView

# Create view
aXYPlot = CreateXYPlotView()

error = 0

aXYPlot.AxisUseCustomRange = [1, 1, 0, 0]
# Left axis range
axisRange = aXYPlot.LeftAxisRange
axisRange[0:2] = [200., 800.]
Ranges = aXYPlot.LeftAxisRange
Etvalue = [200, 800]
error = error + test_values(Ranges, Etvalue)
# Bottom axis range
axisRange = aXYPlot.BottomAxisRange
axisRange[0:2] = [350.,750.]
Ranges = aXYPlot.BottomAxisRange
Etvalue = [350, 750]
error = error + test_values(Ranges, Etvalue)
# Left and bottom axis range
aXYPlot.LeftAxisRange[0:2] = [1350.,2750.]
aXYPlot.BottomAxisRange[0:2] = [240.,2230.]
Etvalue=[1350.,2750.,240.,2230.]
Ranges=aXYPlot.LeftAxisRange
error = error + test_values(Ranges, Etvalue[0:2])
Ranges=aXYPlot.BottomAxisRange
error = error + test_values(Ranges, Etvalue[2:4])
aXYPlot.AxisUseCustomRange = [1, 1, 1, 1]
# Left, bottom, right and top axis range
aXYPlot.LeftAxisRange = [0, 200]
aXYPlot.BottomAxisRange = [100, 450]
aXYPlot.RightAxisRange = [0, 200]
aXYPlot.TopAxisRange = [100, 450]
Etvalue = [0, 200, 100, 450, 0, 200, 100, 450]
Ranges=aXYPlot.LeftAxisRange
error = error + test_values(Ranges, Etvalue[0:2])
Ranges=aXYPlot.BottomAxisRange
error = error + test_values(Ranges, Etvalue[2:4])
Ranges=aXYPlot.RightAxisRange
error = error + test_values(Ranges, Etvalue[4:6])
Ranges=aXYPlot.TopAxisRange
error = error + test_values(Ranges, Etvalue[6:8])

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
