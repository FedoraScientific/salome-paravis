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

from paravistest import *
from presentations import *
from pvsimple import *

# Create view
aXYPlot = CreateXYPlotView()
Render(aXYPlot)

error = 0

# Apply custom axes range
Etvalue = (1, 1, 1, 1)
aXYPlot.LeftAxisUseCustomRange, aXYPlot.BottomAxisUseCustomRange, aXYPlot.RightAxisUseCustomRange, aXYPlot.TopAxisUseCustomRange = 1, 1, 1, 1
Ranges = aXYPlot.LeftAxisUseCustomRange, aXYPlot.BottomAxisUseCustomRange, aXYPlot.RightAxisUseCustomRange, aXYPlot.TopAxisUseCustomRange
error = error + test_values(Ranges, Etvalue)

# Set custom axes range
aXYPlot.LeftAxisUseCustomRange = 1
aXYPlot.BottomAxisUseCustomRange = 1

# Left axis range
Etvalue = (200., 800.)
aXYPlot.LeftAxisRangeMinimum, aXYPlot.LeftAxisRangeMaximum = Etvalue
Ranges = aXYPlot.LeftAxisRangeMinimum, aXYPlot.LeftAxisRangeMaximum
error = error + test_values(Ranges, Etvalue)

# Bottom axis range
Etvalue = (350., 750.)
aXYPlot.BottomAxisRangeMinimum, aXYPlot.BottomAxisRangeMaximum = Etvalue
Ranges = aXYPlot.BottomAxisRangeMinimum, aXYPlot.BottomAxisRangeMaximum
error = error + test_values(Ranges, Etvalue)

# Right axis range
Etvalue = (0., 300.)
aXYPlot.RightAxisRangeMinimum, aXYPlot.RightAxisRangeMaximum = Etvalue
Ranges = aXYPlot.RightAxisRangeMinimum, aXYPlot.RightAxisRangeMaximum
error = error + test_values(Ranges, Etvalue)

# Top axis range
Etvalue = (100., 450.)
aXYPlot.TopAxisRangeMinimum, aXYPlot.TopAxisRangeMaximum = Etvalue
Ranges = aXYPlot.TopAxisRangeMinimum, aXYPlot.TopAxisRangeMaximum
error = error + test_values(Ranges, Etvalue)

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
