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

# This case corresponds to: /visu/SWIG_scripts/A6 case
# Import a table from file and show it in 2D viewer

from paravistest import tablesdir
from presentations import *
import paravis
import pvsimple


# Import table from CSV file
file_path = tablesdir + "sinus.csv"
sinus_csv = pvsimple.CSVReader(FileName=file_path)

# Import settings
sinus_csv.DetectNumericColumns = 1
sinus_csv.UseStringDelimiter = 1
sinus_csv.FieldDelimiterCharacters = ' '
sinus_csv.HaveHeaders = 1

# Display curve
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)
xy_view = pvsimple.CreateXYPlotView()

xy_rep = pvsimple.Show(sinus_csv)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'x'
xy_rep.SeriesVisibility = ['x', '0']

pvsimple.Render()
