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

# This case corresponds to: /visu/ScalarMap_On_DeformedShape/B3 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

#====================Stage1: Import MED file=========================
print 'Importing "fra.med"...................',
medFile = datadir + "fra.med"
my_paravis.ImportFile(medFile)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

view = pvsimple.GetRenderView()
reset_view(view)

#=============Stage2: Create Scalar Map On Deformed Shape=============
field_name = "VITESSE"
node_entity = EntityType.NODE
timestamp = 1

print "Creating Scalar Map On Deformed Shape...................",
smondefshape1 = DeformedShapeAndScalarMapOnField(med_reader, node_entity,
                                                 field_name, 1)

if smondefshape1 is None:
    print "FAILED"
else:
    display_only(smondefshape1, view)
    pvsimple.ResetCamera(view)
    print "OK"

#=============Stage3: Create with 'Y' vector mode========================
hide_all(view)
smondefshape2 = DeformedShapeAndScalarMapOnField(med_reader,
                                                 node_entity,
                                                 field_name, 1,
                                                 vector_mode='Y')

print "Creating Scalar Map On Deformed Shape with 'Y' scalar mode.....",
if smondefshape2 is None:
    print "FAILED"
else:
    print "OK"

#=============Stage4: scaling of Scalar Map On Deformed Shape============
bar = get_bar()
lt = bar.LookupTable

print "Changing scaling mode:"
lt.UseLogScale = 0
print "Log scaling ....", lt.UseLogScale
pvsimple.Render(view)

lt.UseLogScale = 1
print "Log scaling ....", lt.UseLogScale
pvsimple.Render()

#=============Stage5: range of Scalar Map On Deformed Shape==============
print "Changing scalar range:"
range_min = 12
range_max = 120
lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max
print "min = " + str(lt.RGBPoints[0]) + "; max = " + str(lt.RGBPoints[4])
pvsimple.Render(view)

range_min = 120
range_max = 120
lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max
print "min = " + str(lt.RGBPoints[0]) + "; max = " + str(lt.RGBPoints[4])
pvsimple.Render(view)

range_min = 120
range_max = 15
lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max
print "min = " + str(lt.RGBPoints[0]) + "; max = " + str(lt.RGBPoints[4])
pvsimple.Render(view)

#=============Stage6: bar orientation of Scalar Map On Deformed Shape==
print "Changing bar orientation:"
print "Default orientation: ", bar.Orientation

print "Set horizontal bar orientation"
bar.Orientation = 'Horizontal'
print "Bar orientation: ", bar.Orientation
pvsimple.Render(view)

print "Set vertical bar orientation"
bar.Orientation = 'Vertical'
print "Bar orientation: ", bar.Orientation
pvsimple.Render(view)

#=============Stage7: bar position of Scalar Map On Deformed Shape ====
print "Changing bar position:"
print "Default position: ", bar.Position

print "Set position X,Y positive"
x_pos = 200
y_pos = 100
bar.Position = [x_pos, y_pos]
print "Bar position [X,Y]: ", bar.Position
pvsimple.Render(view)

print "Set position X negative ,Y positive"
x_pos = -100
y_pos = 150
bar.Position = [x_pos, y_pos]
print "Bar position [X,Y]: ", bar.Position
pvsimple.Render(view)

print "Set position X positive, Y negative"
x_pos = 100
y_pos = -100
bar.Position = [x_pos, y_pos]
print "Bar position [X,Y]: ", bar.Position
pvsimple.Render(view)

print "Set position X = 0.1, Y = 0.01"
x_pos = 0.1
y_pos = 0.01
bar.Position = [x_pos, y_pos]
print "Bar position [X,Y]: ", bar.Position
pvsimple.Render(view)

#============Stage8: bar size of Scalar Map On Deformed Shape===========
print "Changing bar size:"
print "Default [width, height]: ", bar.Position2

print "Set height = height*1.2"
height = bar.Position2[1] * 1.2
bar.Position2[1] = height
print "Bar [width, height]: ", bar.Position2
pvsimple.Render(view)

print "Set width = width*2"
width = bar.Position2[0] * 2
bar.Position2[0] = width
print "Bar [width, height]: ", bar.Position2
pvsimple.Render(view)

#=============Stage9: nb of colors of Scalar Map On Deformed Shape======
print "Changing number of colors:"
print "Default number of colors: ", lt.NumberOfTableValues
lt.Discretize = 1

print "Set negative number of colors"
num = -128
lt.NumberOfTableValues = num
print "Number of colors: ", lt.NumberOfTableValues
pvsimple.Render(view)

print "Set zero number of colors"
num = 0
lt.NumberOfTableValues = num
print "Number of colors: ", lt.NumberOfTableValues
pvsimple.Render(view)

print "Set positive number of colors"
num = 256
lt.NumberOfTableValues = num
print "Number of colors: ", lt.NumberOfTableValues
pvsimple.Render(view)

#=============Stage10: nb of labels of Scalar Map On Deformed Shape=====
print "Changing number of labels:"
print "Default number of labels: ", bar.NumberOfLabels

print "Set negative number of labels"
num = -128
bar.NumberOfLabels = num
print "Number of labels: ", bar.NumberOfLabels
pvsimple.Render(view)

print "Set zero number of labels"
num = 0
bar.NumberOfLabels = num
print "Number of labels: ", bar.NumberOfLabels
pvsimple.Render(view)

print "Set positive number of labels"
num = 256
bar.NumberOfLabels = num
print "Number of labels: ", bar.NumberOfLabels
pvsimple.Render(view)

#=============Stage11: bar title of Scalar Map On Deformed Shape=========
print "Changing bar title:"
print "Default bar title: ", bar.Title

print "Set not null title"
title = "Scalar Bar Title"
bar.Title = title
print 'Bar title: "' + bar.Title + '"'
pvsimple.Render(view)

print "Set title from spaces"
title = '    '
bar.Title = title
print 'Bar title: "' + bar.Title + '"'
pvsimple.Render(view)

#============Stage12: scale factor of ScalarMap On Deformed Shape =====
print "Changing scale factor:"
warp_vector = smondefshape2.Input

print 'Default scale: ', warp_vector.ScaleFactor

scale = 0.8
warp_vector.ScaleFactor = scale
warp_vector.UpdatePipeline()
print "Scale: ", warp_vector.ScaleFactor
pvsimple.Render(view)
