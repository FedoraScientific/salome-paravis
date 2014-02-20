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

# This case corresponds to: /visu/bugs2/C6 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "bugs/C6")

# 1. Import MED file
med_file_path = datadir + "relachement_brutal_sans_dudg_gauss.med"

print 'Importing "relachement_brutal_sans_dudg_gauss.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError,  "File wasn't imported!!!"
else:
    print "OK"

# 2. Creation of GaussPoints presentations
mesh_name = "Maillage"
field_name = "Result_EQUI_ELGA_SIGM"
timestamp_list = range(1, 5)

for timestamp in timestamp_list:
    print "Creation of the GaussPoints presentation.. Field: ", field_name, "; Timestamp: ", timestamp
    gauss_points = GaussPointsOnField1(med_reader, EntityType.CELL, field_name, timestamp)
    if gauss_points is None:
        raise RuntimeError,  "Created presentation is None!!!"
    pic_path = os.path.join(picturedir, "GaussPoints_" + mesh_name + "_" + field_name + "_" + str(timestamp) + "." + pictureext)
    process_prs_for_test(gauss_points, pvsimple.GetRenderView(), pic_path)


