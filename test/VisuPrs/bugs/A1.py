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

# This case corresponds to: /visu/bugs/A1 case

import sys
import os
import time
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/A1")

med_file_path = datadir + "fra1.med"
pic_path = os.path.join(picturedir, "A1." + pictureext)

# 1. Import MED file
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

# 2. Create mesh
mesh = MeshOnEntity(med_reader, "LE_VOLUME", EntityType.CELL)
mesh.Representation = 'Surface With Edges'

# 3. Display mesh and make snapshot
view = pvsimple.GetRenderView()

display_only(mesh, view)
reset_view(view)

view.CameraViewUp = [0,1,0]
view.CameraPosition = [0, 42, -200]
view.CameraFocalPoint = [25, 0, 5]
view.CameraParallelScale = 1

view.ResetCamera()

pvsimple.Render(view)

print "Picure file name is " + pic_path
pv.WriteImage(pic_path, view=view, Magnification=1)
time.sleep(1)
