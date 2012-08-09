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

# This case corresponds to: /visu/2D_viewer/A1 case

from paravistest import test_values
from pvsimple import CreateXYPlotView

# Create view
aXYPlot = CreateXYPlotView()

# Set range
axisRange = aXYPlot.AxisRange
# Left axis range
axisRange[0:2] = [200., 800.]
Ranges = aXYPlot.AxisRange
Etvalue = [200, 800, axisRange[2], axisRange[3],
           axisRange[4], axisRange[5], axisRange[6], axisRange[7] ]
error = test_values(Ranges, Etvalue)
# Bottom axis range
aXYPlot.AxisRange[2:4] = [350.,750.]
Ranges = aXYPlot.AxisRange
Etvalue = [200, 800, 350, 750,
           axisRange[4], axisRange[5], axisRange[6], axisRange[7]]
error = error + test_values(Ranges, Etvalue)
# Left and bottom axis range
aXYPlot.AxisRange[0:4] = [1350.,2750.,240.,2230.]
Ranges=aXYPlot.AxisRange
Etvalue=[1350.,2750.,240.,2230.,
         axisRange[4], axisRange[5], axisRange[6], axisRange[7]]
error = error + test_values(Ranges, Etvalue)
# Left, bottom, right and top axis range
aXYPlot.AxisRange = [0, 200, 100, 450, 0, 200, 100, 450]
Ranges = aXYPlot.AxisRange
Etvalue = [0, 200, 100, 450, 0, 200, 100, 450]
error = error + test_values(Ranges, Etvalue)

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
