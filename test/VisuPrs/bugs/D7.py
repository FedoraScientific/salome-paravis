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

# This case corresponds to: /visu/bugs3/D7 case

import sys
import os
from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. MED file import
print 'Importing "Fields_group3D.med"........',

med_file_path = datadir + "Fields_group3D.med"
OpenDataFile(med_file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    print "FAILED"
else:
    print "OK"

print 'Get view..............................',
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    print "OK"

# 2. Displaying scalar field
print "Creating Scalar Map.......",

scalarmap = ScalarMapOnField(med_reader, EntityType.CELL, 'scalar field', 1)
if scalarmap is None:
    print "FAILED"
else:
    print "OK"

med_reader.Groups = ['GROUP/mailles_MED/OnCell/box_1']

display_only(scalarmap, view)
reset_view(view)

