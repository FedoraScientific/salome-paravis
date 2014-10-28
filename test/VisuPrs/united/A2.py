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

# This case corresponds to: /visu/united/A2 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple
from math import radians

my_paravis = paravis.myParavis

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

# 1. Import MED file
print "**** Step1: Import MED file"

print 'Import "fra.med"....................',
file_path = datadir + "fra.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

view = pvsimple.GetRenderView()

field_name = 'VITESSE'
print "Creating Cut Planes.................",
orient = Orientation.YZ
cutplanes = CutPlanesOnField(med_reader, node_entity, field_name, 1, orientation=orient)
if cutplanes is None:
    print "FAILED"
else:
    display_only(cutplanes, view)
    print "OK"

slice_filter = pvsimple.GetActiveSource()

# 2. Changing view
print "**** Step2: Changing view"

print "Fit All.............................",
reset_view(view)
print "OK"

print "Rotate..............................",
x_angle = 0
y_angle = 0

nb_planes = len(slice_filter.SliceOffsetValues)

for i in xrange(1, 50):
    y_angle = y_angle + 0.05
    normal = get_normal_by_orientation(orient, x_angle, radians(y_angle))
    slice_filter.SliceType.Normal = normal
    pvsimple.Render()

for i in xrange(1,50):
    y_angle = y_angle - 0.05
    normal = get_normal_by_orientation(orient, x_angle, radians(y_angle))
    slice_filter.SliceType.Normal = normal
    pvsimple.Render()
    
print "OK"

print "View Point (FRONT)..................",
view.CameraViewUp = [0.0, 0.0, 1.0]
view.CameraPosition = [2.4453961849843453, 0.03425, 0.541]
pvsimple.Render()
print "OK"

print "View Point (BACK)...................", 
view.CameraPosition = [-2.0343961849843457, 0.03425, 0.541]
pvsimple.Render()
print "OK"

print "View Point (TOP)....................",
view.CameraViewUp = [0.0, 1.0, 0.0]
view.CameraPosition = [0.2055, 0.03425, 2.7808961849843454]
pvsimple.Render()
print "OK"

print "View Point (BOTTOM).................",
view.CameraPosition = [0.2055, 0.03425, -1.6988961849843456]
pvsimple.Render()
print "OK"

print "View Point (LEFT)...................",
view.CameraViewUp = [0.0, 0.0, 1.0]
view.CameraPosition = [0.2055, -2.2056461849843454, 0.541]
pvsimple.Render()
print "OK"

print "View Point (RIGHT) .................", 
view.CameraPosition = [0.2055, 2.2741461849843456, 0.541]
pvsimple.Render()
print "OK"

print "Restore View........................",
reset_view(view)
print "OK"
