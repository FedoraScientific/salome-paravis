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

# This case corresponds to: /visu/DeformedShape/B4 case

from paravistest import datadir
from presentations import DeformedShapeOnField, EntityType
import paravis
import pvsimple


my_paravis = paravis.myParavis

#====================Stage1: Import from MED file in ParaVis============
print "**** Stage1: Import from MED file in ParaVis"

print 'Import "Hexa8.med"....................',

file_path = datadir + "Hexa8.med"
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

#====================Stage3: Color of Deformed Shape====================
print "**** Stage3: Color of Deformed Shape"

# Step1
print "Getting the current color of Deformed Shape in RGB ... ",
color = defshape.AmbientColor
print color

# Step2
print "Set the positive color in RGB"
color = [0.3, 0.3, 0.3]
defshape.AmbientColor = color
pvsimple.Render()

color = defshape.AmbientColor
print "Color: ", color

# Step3
print "Set the negative R and positive GB"
color = [-0.3, 0.3, 0.3]
defshape.AmbientColor = color
pvsimple.Render()

color = defshape.AmbientColor
print "Color: ", color

# Step4
print "Set the negative R and positive GB"
color = [0.3, -0.3, 0.3]
defshape.AmbientColor = color
pvsimple.Render()

color = defshape.AmbientColor
print "Color: ", color

# Step5
print "Set the negative B and positive RG"
print "Set the negative R and positive GB"
color = [0.3, 0.3, -0.3]
defshape.AmbientColor = color
pvsimple.Render()

color = defshape.AmbientColor
print "Color: ", color
