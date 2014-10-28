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

# This case corresponds to: /visu/CutPlanes/B3 case

from paravistest import datadir
from presentations import CutPlanesOnField, EntityType
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

#====================Stage1: Importing MED file====================
print "**** Stage1: Importing MED file"

print 'Import "ResOK_0000.med"...............',

file_path = datadir + "ResOK_0000.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

#====================Stage2: Creating CutPlanes====================
print "*****Stage2: Creating CutPlanes"

print "Creating Cut Planes.......",

node_entity = EntityType.NODE
field_name = 'vitesse'
cutplanes = CutPlanesOnField(med_reader, node_entity, field_name, 1)

if cutplanes is None:
    print "FAILED"
else:
    print "OK"
