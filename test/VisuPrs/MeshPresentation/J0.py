# Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

#This case corresponds to: /visu/MeshPresentation/J0 case
# Create  mesh presentation for nodes and cells of the the given MED file 

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "MeshPresentation/J0")

theFileName = datadir +  "DividedGenTetra1.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "


result = myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

aView = GetRenderView()

#%Creation of the mesh presentation%
mesh_name = "DividedGenTetra1"

#^Presentation on "onNodes" and '"onCells" family^
entity_types = [EntityType.NODE,EntityType.CELL]
for entity_type in entity_types:
    entity_name = EntityType.get_name(entity_type)
    mesh = MeshOnEntity(aProxy, mesh_name ,entity_type)
    if mesh is None: 
        msg = "ERROR!!!Presentation of mesh on '"+entity_name+"' family wasn't created..."
        raise RuntimeError, msg 
    mesh.Visibility=1
    reset_view(aView)
    Render()

    # Add path separator to the end of picture path if necessery
    if not picturedir.endswith(os.sep):
            picturedir += os.sep
    entity_name = EntityType.get_name(entity_type)
    # Construct image file name
    pic_name = picturedir + mesh_name + "_" + entity_name + "." + pictureext
    
    process_prs_for_test(mesh, aView, pic_name)



