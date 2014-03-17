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

# This case corresponds to: /visu/bugs3/D6 case

from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
med_file_path = datadir + "fra.med"

my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

# 2. Creation of CutSegment presentations, based on time stamps of "VITESSE" field
point1 = [0.0, 0.0, 0.0]
point2 = [0.4, 0.05, 1.0]
cut_segment1 = CutSegmentOnField(med_reader, EntityType.NODE, "VITESSE", 1,
                                point1, point2)
if cut_segment1 == None:
   raise RuntimeError, "ERROR!!! The first CutSegment presentation is not built!" 

# 3. Creation of CutSegment presentations, based on time stamps of "TAUX_DE_VIDE" field
point1 = [0, 0, 1]
point2 = [0.2055, 0.0685, 0.541]
cut_segment2 = CutSegmentOnField(med_reader, EntityType.NODE, "TAUX_DE_VIDE", 1,
                                point1, point2)
if cut_segment2 == None:
   raise RuntimeError, "ERROR!!! The second CutSegment presentation is not built!"

# 4. Creation of Table based on CutSegment presentation

# TODO: it's possible to display CutSegment (PlotOverLine in terms of ParaViS) presentation
#       as a table in GUI, but it seems that there is no possibility to create table view
#       (spreadsheet view in terms of ParaViS) from in Python script

# 5. Creation of curve based on Table
xy_view = pvs.CreateXYPlotView()

curve1 = pvs.Show(cut_segment1.Input, xy_view)
if curve1 == None:
   raise RuntimeError, " ERROR!!! Curve based on the first CutSegment is not built!"

curve2 = pvs.Show(cut_segment2.Input, xy_view)
if curve2 == None:
   raise RuntimeError, " ERROR!!! Curve based on the second CutSegment is not built!"

curves = [curve1, curve2]
for c in curves:
    c.AttributeType = 'Point Data'
    c.UseIndexForXAxis = 0
    c.XArrayName = 'arc_length'

pvsimple.SetActiveSource(cut_segment1.Input)
set_visible_lines(curve1, ['VITESSE (Magnitude)'])

pvsimple.SetActiveSource(cut_segment2.Input)
set_visible_lines(curve2, ['TAUX_DE_VIDE'])

pvsimple.ResetCamera(xy_view)
