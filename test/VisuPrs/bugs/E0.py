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

# This case corresponds to: /visu/bugs4/E0 case

import time
from paravistest import datadir
from presentations import *
import paravis
import pvsimple

sleep_delay = 1
my_paravis = paravis.myParavis

# 1. MED file import
print 'Import "ResOK_0000.med"...............',
med_file_path = datadir + "ResOK_0000.med"
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError, "ResOK_0000.med was not imported!!!"
else:
    print "OK"

# TODO: in the original VISU script the timestamp id was equal to 2,
#       but in PARAVIS it's inly one timestamp accessible
timestamp_id = 1

# 2. Create Scalar Map
field_name = 'vitesse'

print "Creating Scalar Map......."
scmap1 = ScalarMapOnField(med_reader, EntityType.NODE, field_name, timestamp_id)
if scmap1 is None : 
    raise RuntimeError, "ScalarMap presentation is None!!!"
else:
    print "OK"

display_only(scmap1)
reset_view()

print "WIREFRAME sur scmap1"
scmap1.Representation = 'Wireframe'
time.sleep(sleep_delay)

print "POINT sur scmap1"
scmap1.Representation = 'Points'
time.sleep(sleep_delay)

print "SURFACEFRAME sur scmap1"
scmap1.Representation = 'Surface With Edges'
time.sleep(sleep_delay)

# 3. Create Deformed Shape And Scalar Map
print "Creating DeformedShapeAndScalarMap......."
scmap2 = DeformedShapeAndScalarMapOnField(med_reader, EntityType.NODE, field_name, timestamp_id)
if scmap2 is None : 
    raise RuntimeError, "DeformedShapeAndScalarMapOnField presentation is None!!!"
else:
    print "OK"
    
scmap2.Input.ScaleFactor = 1.0

display_only(scmap2)
reset_view()

print "WIREFRAME sur scmap2"
scmap2.Representation = 'Wireframe'
time.sleep(sleep_delay)

print "POINT sur scmap2"
scmap2.Representation = 'Points'
time.sleep(sleep_delay)

print "SURFACEFRAME sur scmap2"
scmap2.Representation = 'Surface With Edges'
time.sleep(sleep_delay)

# 4.Create Deformed Shape
print "Creating DeformedShape........"
scmap3 = DeformedShapeOnField(med_reader, EntityType.NODE, field_name, timestamp_id)
if scmap3 is None : 
    raise RuntimeError, "DeformedShapeOnField presentation is None!!!"
else:
    print "OK"

print "WIREFRAME sur scmap3"
scmap3.Representation = 'Wireframe'
time.sleep(sleep_delay)

print "POINT sur scmap3"
scmap3.Representation = 'Points'
time.sleep(sleep_delay)

print "SURFACEFRAME sur scmap3"
scmap3.Representation = 'Surface With Edges'
time.sleep(sleep_delay)

scmap2.Input.ScaleFactor = 1.0

# show colored:
scmap3.ColorAttributeType = EntityType.get_pvtype(EntityType.NODE)
scmap3.ColorArrayName = field_name

display_only(scmap3)
reset_view()



