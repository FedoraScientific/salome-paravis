
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

# This case corresponds to: /visu/SWIG_scripts/B4 case

import sys
import os

from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

# Import MED file
med_file_path = datadir + "pointe.med"
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "MED file was not imported successfully."

# Create presentations
try:
    if os.access(med_file_path, os.R_OK) :
       if os.access(med_file_path, os.W_OK) :
           mesh_name = "maa1"
           entity = EntityType.NODE
           field_name = "fieldnodedouble"
           timestamp_id = 1
           
           scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp_id)
           if get_nb_components(med_reader, entity, field_name) > 1:
               vectors = VectorsOnField(med_reader, entity, field_name, timestamp_id)
               cutplanes = CutPlanesOnField(med_reader, entity, field_name, timestamp_id)

           mesh = MeshOnEntity(med_reader, mesh_name, entity)
       else:
           print "We have no permission to rewrite medFile"
    else:
        print  "We have no permission to read medFile, it will not be opened"; 
except:
    print sys.exc_type 
    print sys.exc_value
    print sys.exc_traceback


