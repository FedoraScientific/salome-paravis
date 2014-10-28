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

# This case corresponds to: /visu/united1/B0 case

from paravistest import datadir, compare_lists
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

error=0

# 1. Import MED file
file_path = datadir + "TimeStamps.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Create scalar map on deformed shape
smondefshape = DeformedShapeAndScalarMapOnField(med_reader, EntityType.NODE, 'vitesse', 1)
if smondefshape is None:
    print "FAILED"
else:
    print "OK"

# 3. Make compare

# Offset
smondefshape.Position = [0, 0, 0]
error = error + compare_lists(smondefshape.Position, [0,0,0])

# Scalar mode
smondefshape.LookupTable.VectorMode = 'Magnitude'
if smondefshape.LookupTable.VectorMode != 'Magnitude':
    print "ScalarMode is wrong..."
    error=error+1

# Scalar bar Position
bar = get_bar()
bar.Position = [0.01, 0.01]
error = error + compare_lists(bar.Position, [0.01, 0.01])

# Scalar bar size
bar.Position2 = [0.05, 0.5]
error = error + compare_lists(bar.Position2, [0.05, 0.5])

# Number of colors and labels
smondefshape.LookupTable.NumberOfTableValues = 4
bar.NumberOfLabels = 5

nb_colors = smondefshape.LookupTable.NumberOfTableValues
nb_labels = bar.NumberOfLabels
error = error + compare_lists([nb_colors, nb_labels], [4, 5])

# Title
bar.Title = 'Pression, Pa'
bar.ComponentTitle = "Comp."

if bar.Title != 'Pression, Pa':
    print "Title is wrong..."
    error=error+1

if bar.ComponentTitle!= 'Comp.':
    print "Component title is wrong..."
    error=error+1

# Scaling
smondefshape.LookupTable.UseLogScale = 0
if smondefshape.LookupTable.UseLogScale != 0:
    print "Error!!! Scaling is wrong"
    error=error+1

# Bar orientation
bar.Orientation = 'Horizontal'
if bar.Orientation != 'Horizontal':
    print "ERROR!!! Scalar Bar orientation is wrong..."
    error=error+1

# Scale factor
smondefshape.Input.ScaleFactor = 0.15
error = error + compare_lists([smondefshape.Input.ScaleFactor], [0.15])


if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
