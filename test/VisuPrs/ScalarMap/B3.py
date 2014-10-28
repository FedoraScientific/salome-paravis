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

# This case corresponds to: /visu/ScalarMap/B3 case
# Test ScalarMap interface methods.

from paravistest import datadir
import presentations
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file

print 'Importing "fra.med"...',
file_path = datadir + "fra.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Create ScalarMap
field_name = 'TAUX_DE_VIDE'
entity =  presentations.EntityType.NODE
myMeshName='LE VOLUME'

scalarmap = presentations.ScalarMapOnField(med_reader, entity, field_name, 1)
if scalarmap is None:
    print "FAILED"

# 3. Scalar mode
lookup_table = scalarmap.LookupTable
print "Vector mode .....", lookup_table.VectorMode
print "Vector component .....", lookup_table.VectorComponent

lookup_table.VectorMode = 'Component'
lookup_table.VectorComponent = 0

scalarmap.LookupTable = lookup_table 

print "Vector mode .....", scalarmap.LookupTable.VectorMode
print "Vector component .....", scalarmap.LookupTable.VectorComponent

# 4. Scaling mode
scalarmap.LookupTable.UseLogScale = 1
print "Use logarithmic scaling ....", scalarmap.LookupTable.UseLogScale

scalarmap.LookupTable.UseLogScale = 0
print "Use logarithmic scaling ....", scalarmap.LookupTable.UseLogScale

# 5. Scalar range
print "Set scalar range min=12 < max=120 ...",
rmin = 12
rmax = 120
scalarmap.LookupTable.RGBPoints[0] = rmin
scalarmap.LookupTable.RGBPoints[4] = rmax
print "min = ", scalarmap.LookupTable.RGBPoints[0],"  : max = ",scalarmap.LookupTable.RGBPoints[4]

print "Set scalar range min=max=12 ...",
rmin = 120
rmax = rmin
scalarmap.LookupTable.RGBPoints[0] = rmin
scalarmap.LookupTable.RGBPoints[4] = rmax
print "min = ", scalarmap.LookupTable.RGBPoints[0],"  : max = ",scalarmap.LookupTable.RGBPoints[4]

print "Set scalar range min=120 > max=15 ...",
rmin = 120
rmax = 15
scalarmap.LookupTable.RGBPoints[0] = rmin
scalarmap.LookupTable.RGBPoints[4] = rmax
print "min = ", scalarmap.LookupTable.RGBPoints[0],"  : max = ",scalarmap.LookupTable.RGBPoints[4]

# 6. Bar orientation
bar = presentations.get_bar()

print "Set bar orientation = 'Horizontal'"
bar.Orientation = 'Horizontal'
print "Bar orientation ....", bar.Orientation

print "Set bar orientation = 'Vertical'"
bar.Orientation = 'Vertical'
print "Bar orientation ....", bar.Orientation

# 7. Position of scalar bar
print "Default position ....", bar.Position

print "Set left down corner position"
bar.Position = [0, 0]
print "Position =", bar.Position

print "Set position outside of the screen"
bar.Position = [-1, -1]
print "Position =", bar.Position

# 8. Size of scalar bar
print "Default Height=", bar.Position2[1],"  :  Width=", bar.Position2[0]

print "Set positive Height and Width"
h = 0.4
w = 0.2
bar.Position2 = [w, h]
print "Size =", bar.Position2

print "Set negative Height and Width"
h = -0.4
w = -0.2
bar.Position2 = [w, h]
print "Size =", bar.Position2

# 9. Number of colors
print "Default number of colors = ", scalarmap.LookupTable.NumberOfTableValues

scalarmap.LookupTable.Discretize = 1

print "Set negative number of colors"
nb_colors = -128
scalarmap.LookupTable.NumberOfTableValues = nb_colors
print "Number of colors =", scalarmap.LookupTable.NumberOfTableValues

print "Set zero number of colors"
nb_colors = 0
scalarmap.LookupTable.NumberOfTableValues = nb_colors
print "Number of colors =", scalarmap.LookupTable.NumberOfTableValues

print "Set positive number of colors"
nb_colors = 256
scalarmap.LookupTable.NumberOfTableValues = nb_colors
print "Number of colors =", scalarmap.LookupTable.NumberOfTableValues

# 10. Number of labels
print "Default number of labels = ", bar.NumberOfLabels

print "Set negative number of labels"
nb_labels = -10
bar.NumberOfLabels = nb_labels
print "Number of labels=", bar.NumberOfLabels

print "Set zero number of labels"
nb_labels = 0
bar.NumberOfLabels = nb_labels
print "Number of labels=", bar.NumberOfLabels

print "Set positive number of labels"
nb_labels = 10
bar.NumberOfLabels = nb_labels
print "Number of labels=", bar.NumberOfLabels

# 11. Scalar bar title
print 'Default Title ="', bar.Title, '"'

print "Set not null title"
title = "Scalar Bar Title"
bar.Title = title
print '   Title ="', bar.Title, '"'

print "Set title from spaces"
title='    '
bar.Title = title
print '   Title ="', bar.Title, '"'
