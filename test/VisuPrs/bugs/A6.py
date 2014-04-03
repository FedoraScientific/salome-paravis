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

# This case corresponds to: /visu/bugs/A6 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/A6")

med_file_path = datadir + "fra.med"

# 1. Import MED file
print 'Importing "fra.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "Error"
else:
    print "OK"

# 2. Create mesh
mesh_names = get_mesh_names(med_reader)
for mesh_name in mesh_names:
    print "Mesh name: ", mesh_name
    mesh = MeshOnEntity(med_reader, mesh_name, EntityType.CELL)
    if mesh is None:
        raise RuntimeError, "Error"

    mesh.Representation = 'Wireframe'
        
    pic_path = os.path.join(picturedir, mesh_name + "_Cell." + pictureext)
    pic_path = re.sub("\s+","_", pic_path)
    print "Save picture ", pic_path
    process_prs_for_test(mesh, pvsimple.GetRenderView(), pic_path)
    

