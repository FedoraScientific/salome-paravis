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

# This case corresponds to: /visu/bugs2/C3 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
med_file_path = datadir + "MEDfileForStructuredMesh.med"

print 'Importing "MEDfileForStructuredMesh.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "MEDfileForStructuredMesh.med was not imported!"
else:
    print "OK"

errors = 0

# 2. Creation of the mesh presentations for the "AssemblyMesh" mesh
mesh_name = "AssemblyMesh"

for entity in [EntityType.NODE, EntityType.CELL]:
    if MeshOnEntity(med_reader, mesh_name, entity) is None:
        print "Entity:", str(entity)
        print "ERROR!!! Mesh presentation for \""+mesh_name+"\" wasn't created!!!"
        errors += 1

# 3. Creation of the mesh presentations for the "CoreMesh" mesh
mesh_name = "CoreMesh"

for entity in [EntityType.NODE, EntityType.CELL]:
    if MeshOnEntity(med_reader, mesh_name, entity) is None:
        print "Entity:", str(entity)
        print "ERROR!!! Mesh presentation for \""+mesh_name+"\" wasn't created!!!"
        errors += 1

if errors > 0:
    raise RuntimeError, "Some errors were occured during execution... See ERRORs above for details!"
