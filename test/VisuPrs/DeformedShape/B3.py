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

# This case corresponds to: /visu/DeformedShape/B3 case

from paravistest import datadir
from presentations import DeformedShapeOnField, EntityType
import pvserver as paravis
import pvsimple


my_paravis = paravis.myParavis

#====================Stage1: Import from MED file in ParaVis============
print "**** Stage1: Import from MED file in ParaVis"

print 'Import "Tria3.med"....................',

file_path = datadir + "Tria3.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

#====================Stage2: Creating Deformed Shape====================
print "**** Stage2: Creating Deformed Shape"

print "Creating Deformed Shape...............",

cell_entity = EntityType.CELL
field_name = 'vectoriel field'
defshape = DeformedShapeOnField(med_reader, cell_entity, field_name, 1)
pvsimple.ResetCamera()

if defshape is None:
    print "FAILED"
else:
    print "OK"

#====================Stage3: Scale of Deformed Shape====================
print "**** Stage3: Scale of Deformed Shape"

warp_vector = pvsimple.GetActiveSource()
print "Default scale: ", warp_vector.ScaleFactor

print "Set positive scale of Deformed Shape"
scale = 1
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

print "Set negative scale of Deformed Shape"
scale = -1
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

print "Set zero scale of Deformed Shape"
scale = 0
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

#====================Stage4: Coloring method of Deformed Shape===========
print "**** Stage4: Coloring of Deformed Shape"

color_array = defshape.ColorArrayName
if color_array:
    print "Default shape is colored by array: ", color_array
else:
    print "Default shape is colored by solid color: ", defshape.AmbientColor

print "Set colored by array mode    .... ",
defshape.ColorArrayName = field_name
pvsimple.Render()

if defshape.ColorArrayName == field_name:
    print "OK"
else:
    print "FAILED"

print "Set colored by solid color mode .... ",
defshape.ColorArrayName = ''
pvsimple.Render()

if defshape.ColorArrayName:
    print "FAILED"
else:
    print "OK"
