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

# This case corresponds to: /visu/united/A5 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

# 1. Import MED file
print "**** Step1: Import MED file"

print 'Import "ResOK_0000.med"...............',
file_path = datadir + "ResOK_0000.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

mesh_name = 'dom'

# 2. Creating mesh
print "**** Step2: Mesh creation"

print "Creating mesh.............",
mesh = MeshOnEntity(med_reader, mesh_name, cell_entity)
if mesh is None:
    print "FAILED"
else:
    print "OK"

# 3. Changing type of presentation of mesh
print "**** Step3: Changing type of presentation of mesh"

view = pvsimple.GetRenderView()
display_only(mesh, view)
reset_view(view)

mesh.Representation = 'Wireframe'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

mesh.Representation = 'Points'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

# make shrink, in PARAVIS it's not a representation type: use shrink filter
shrink = pvsimple.Shrink(med_reader)
mesh_shrinked = pvsimple.GetRepresentation(shrink)
display_only(mesh_shrinked, view)

display_only(mesh, view)
mesh.Representation = 'Surface With Edges'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

# 4. Changing Cell color of mesh
print "**** Step4: Changing Cell color of mesh"

color = mesh.DiffuseColor
print "Mesh Cell color in RGB....(", color[0], ",", color[1], ",", color[2], ")"

color = [0, 0, 1]
mesh.DiffuseColor = color
color = mesh.DiffuseColor
print "Mesh Cell color in RGB....(", color[0], ",", color[1], ",", color[2], ")"
pvsimple.Render()

# 5. Changing Node color of mesh
print "**** Step4: Changing Node color of mesh"

color = mesh.AmbientColor
print "Mesh Node color in RGB....(", color[0], ",", color[1], ",", color[2], ")"

color = [0, 1, 0]
mesh.AmbientColor = color
color = mesh.AmbientColor
print "Mesh Node color in RGB....(", color[0], ",", color[1], ",", color[2], ")"
pvsimple.Render()

## Note: no special property for edge color in PARAVIS; skip link color changing ( SetLinkColor()/GetLinkColor() methods )

