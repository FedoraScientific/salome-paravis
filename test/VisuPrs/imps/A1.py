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

# This case corresponds to: /visu/imps/A1 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "imps/A1")


def set_prs_colored(prs, proxy, entity, field_name, vector_mode, timestamp_nb):
    # Get time value
    time_value = get_time(proxy, timestamp_nb)
    
    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    prs.ColorAttributeType = EntityType.get_pvtype(entity)
    prs.ColorArrayName = field_name
    prs.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)
    

# 1. Import of the "Penta6.med" file
print 'Import "Penta6.med" file........',
file_path = datadir + "Penta6.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError, "Penta6.med was not imported!!!"
else:
    print "OK"

view = pvsimple.GetRenderView()

# 2. Creation of "CutPlanes" presentation, based on time stamp of "scalar field" field
print 'Creation of "CutPlanes" presentation, based on time stamp of "scalar field" field....'
cutplanes = CutPlanesOnField(med_reader, EntityType.CELL, "scalar_field", 1)
if cutplanes is None : 
    raise RuntimeError, "Presentation is None!!!"
else:
    print "OK"

print "Setting of deformation:"
warp_vector = pvsimple.WarpByVector(cutplanes.Input)
warp_vector.Vectors = ["vectoriel_field"]
warp_vector.ScaleFactor = 5.0

print "Got scale     : ", warp_vector.ScaleFactor
print "Got field name: ", warp_vector.Vectors

presentation = pvsimple.GetRepresentation(warp_vector)
set_prs_colored(presentation, med_reader, EntityType.CELL, "scalar_field", 'Magnitude', 1)

pic_path = os.path.join(picturedir, "deformed_cut_planes_scalar" + "." + pictureext)
process_prs_for_test(presentation, view, pic_path)

# 3. Creation of "CutPlanes" presentation, based on time stamp of "vectoriel field" field
print 'Creation of "CutPlanes" presentation, based on time stamp of "vectoriel field" field....'
cutplanes = CutPlanesOnField(med_reader, EntityType.CELL, "vectoriel_field", 1)

if cutplanes is None : 
    raise RuntimeError, "Presentation is None!!!"
else:
    print "OK"

print "Setting of deformation:"
warp_vector = pvsimple.WarpByVector(cutplanes.Input)
warp_vector.Vectors = ["vectoriel_field"]
warp_vector.ScaleFactor = 5.0

print "Got scale     : ", warp_vector.ScaleFactor
print "Got field name: ", warp_vector.Vectors

presentation = pvsimple.GetRepresentation(warp_vector)
set_prs_colored(presentation, med_reader, EntityType.CELL, "vectoriel_field", 'Magnitude', 1)

pic_path = os.path.join(picturedir, "deformed_cut_planes_vectorial" + "." + pictureext)
process_prs_for_test(presentation, view, pic_path)
