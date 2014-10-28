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

# This case corresponds to: /visu/bugs2/C9 case

from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
med_file_path = datadir + "TimeStamps.med"

my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "TimeStamps.med was not imported!!!"

# 2. Creation of presentations
mesh_on_cells = MeshOnEntity(med_reader, "dom", EntityType.CELL)
if mesh_on_cells is None : 
    raise RuntimeError, "Mesh presentation is None!!!"

view = pvsimple.GetRenderView()
display_only(mesh_on_cells, view)
reset_view(view)

mesh_on_cells.Representation = 'Wireframe'

scalar_map = ScalarMapOnField(med_reader, EntityType.NODE, "vitesse", 1)
if scalar_map is None : 
    raise RuntimeError, "ScalarMap presentation is None!!!"

scalar_map.Visibility = 1
pvsimple.Render()

pvsimple.Delete(scalar_map)
pvsimple.Render()
