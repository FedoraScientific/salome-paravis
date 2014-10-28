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

# This case corresponds to: /visu/bugs3/D3 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import of the "LinearStaticsSTA9.resu.med" file
med_file_path = datadir + "LinearStaticsSTA9.resu.med"
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError, "LinearStaticsSTA9.resu.med was not imported!!!"

view = pvsimple.GetRenderView()

# Creation of colored "DeformedShape" presentations, based on time stamps of "RESU_DEPL" field
presentation = DeformedShapeOnField(med_reader, EntityType.NODE, 'RESU____DEPL____________________', 1, is_colored=True)
if presentation is None : 
    raise RuntimeError, "DeformedShapeOnField Presentation is None!!!"

display_only(presentation, view)
reset_view(view)
  
# Creation of colored "Vectors" presentations, based on time stamps of "RESU_DEPL" field
presentation = VectorsOnField(med_reader, EntityType.NODE, 'RESU____DEPL____________________', 1, is_colored=True)
if presentation is None : 
    raise RuntimeError, "Vectors Presentation is None!!!"

display_only(presentation, view)
reset_view(view)

# Creation of colored "DeformedShapeAndScalarMap" presentations, based on time stamps of "RESU_DEPL" field
presentation = DeformedShapeAndScalarMapOnField(med_reader, EntityType.NODE, 'RESU____DEPL____________________', 1)
if presentation is None : 
    raise RuntimeError, "DeformedShapeAndScalarMap Presentation is None!!!"

display_only(presentation, view)
reset_view(view)

# Creation of colored "CutPlanes" presentations, based on time stamps of "RESU_DEPL" field
presentation = CutPlanesOnField(med_reader, EntityType.NODE, 'RESU____DEPL____________________', 1)
if presentation is None : 
    raise RuntimeError, "CutPlanes Presentation is None!!!"

display_only(presentation, view)
reset_view(view)


