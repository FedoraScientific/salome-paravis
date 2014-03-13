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

# This case corresponds to: /visu/bugs3/D0 case

import sys
import os
import time
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/D0")

# Aux method
def get_group_full_name(source, group_name):
    result_name = group_name
    
    full_names = source.Groups.Available
    for name in full_names:
        if name.endswith(group_name):
            result_name = name
            break

    return result_name

# 1. Import of the "Bug619-result_calcul_OCC.med" file
med_file_path = datadir + "Bug619-result_calcul_OCC.med"

my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "Bug619-result_calcul_OCC.med was not imported!!!"

# 2. Creation of ScalarMap:
# iteration1: on the "TU_3D_G1" group
# iteration2: on the "TU_3D_D1" group
view = pvsimple.GetRenderView()
field_name = "MECASTATEQUI_ELNO_SIGM"

groups = ['TU_3D_G1', 'TU_3D_D1']

for group_name in groups:
    extract_group = pvsimple.ExtractGroup(med_reader)
    extract_group.Groups = [get_group_full_name(med_reader, group_name)]
    extract_group.UpdatePipeline()
    
    scalar_map = ScalarMapOnField(extract_group, EntityType.CELL, field_name, 1)
    if scalar_map is None :
        raise RuntimeError, "ScalarMap presentation on '" + group_name + "' group is None!!!"

    pic_path = os.path.join(picturedir, "npal18711_" + group_name + "." + pictureext)
    process_prs_for_test(scalar_map, view, pic_path)
    
