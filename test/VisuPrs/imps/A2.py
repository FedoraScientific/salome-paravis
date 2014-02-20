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

# This case corresponds to: /visu/imps/A2 case

import time
from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
sleep_delay = 2

med_file = "pointe.med"
entity = EntityType.NODE
field_name = "fieldnodedouble";
timestamp = 1

# 1. Import MED file
print 'Import "pointe.med"....................',
med_file_path = datadir + med_file
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Create Scalar Map
print "Build Scalar Map presentation"
scalar_map = ScalarMapOnField(med_reader, entity, field_name, timestamp)

display_only(scalar_map)
reset_view()
time.sleep(sleep_delay)

# 3. Set representation type to Point
print "Set representation type to Point"
scalar_map.Representation = 'Points'
pvsimple.Render()
time.sleep(sleep_delay)

# 4. Set representation type to Point Sprite
print "Set Point Sprite representation"
scalar_map.Representation = 'Point Sprite'

data_range = get_data_range(med_reader, entity,
                            field_name, 'Magnitude')
mult = abs(0.1 / data_range[1])
scalar_map.RadiusScalarRange = data_range
scalar_map.RadiusTransferFunctionEnabled = 1
scalar_map.RadiusMode = 'Scalar'
scalar_map.RadiusArray = ['POINTS', field_name]
scalar_map.RadiusTransferFunctionMode = 'Table'
scalar_map.RadiusScalarRange = data_range
scalar_map.RadiusUseScalarRange = 1
scalar_map.RadiusIsProportional = 1
scalar_map.RadiusProportionalFactor = mult

pvsimple.Render()


