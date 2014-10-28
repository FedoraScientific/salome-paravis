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

# This case corresponds to: /visu/bugs2/C7 case

import os
import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/C7")

# 1. Import MED file
med_file_path = datadir + "Bug583-Quadratique.resu.med"

print 'Importing "Bug583-Quadratique.resu.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "File wasn't imported!!!"
else:
    print "OK"

# 2. Creation of GaussPoints presentations
mesh_name = "MAIL"
field_names = ["RESU_EQUI_ELNO_SIGM", "RESU_SIEF_ELGA_DEPL", "RESU_SIEF_ELNO_ELGA", "RESU_SIGM_ELNO_DEPL"]

view = pvsimple.GetRenderView()

print "BREAKPOINT_1"
error = 0

for field in field_names:
    print "Creation of the GaussPoints presentation.. Field: ", field, "; Iteration: 1"
    presentation = GaussPointsOnField1(med_reader, EntityType.CELL, field, 1)
    if presentation is None:
	print "ERROR!!! GaussPoints presentation wasn't created for the ", field, " field!!!"
        error += 1
    else:
        pic_path = os.path.join(picturedir, "GaussPoints_" + mesh_name + "_" + field + "." + pictureext)
        process_prs_for_test(presentation, view, pic_path)

if not error:
    print "BREAKPOINT_2"
else:
    raise RuntimeError, "There are some errors were occured... For more info see ERRORs above..."
