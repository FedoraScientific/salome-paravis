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

# This case corresponds to: /visu/bugs/A3 case

import time
from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
med_file_path = datadir + "hexa_28320_ELEM.med"
OpenDataFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

# 2. Create vectors
vectors = VectorsOnField(med_reader, EntityType.CELL, "vitesse_elem_dom_pb1", 2) # 1e-05

display_only(vectors)
reset_view()
time.sleep(1)

vectors.Input.SetScaleFactor = 0.005
display_only(vectors)
reset_view()
time.sleep(1)


vectors.Input.SetScaleFactor = 0.002
display_only(vectors)
reset_view()
time.sleep(1)
