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

# This case corresponds to: /visu/SWIG_scripts/B9 case

from paravistest import datadir
from presentations import EntityType, ScalarMapOnField, CutPlanesOnField
import pvserver as paravis
import pvsimple


my_paravis = paravis.myParavis

# Get view
view1 = pvsimple.GetRenderView()

# Import MED file
med_file = datadir + "pointe.med"
field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1

OpenDataFile(med_file)
med_reader = pvsimple.GetActiveSource()

# Create presentations
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
pvsimple.ResetCamera(view1)

view2 = pvsimple.CreateRenderView()
cutlines = CutPlanesOnField(med_reader, entity, field_name, timestamp)
pvsimple.ResetCamera(view2)

# Delete
source = cutlines.Input
pvsimple.Delete(source)
pvsimple.Delete(med_reader)

# Clear views from scalar bar and update views
for rview in pvsimple.GetRenderViews():
    rview.Representations.Clear()
    pvsimple.Render(rview)
