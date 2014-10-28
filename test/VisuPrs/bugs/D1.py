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

# This case corresponds to: /visu/bugs3/D1 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/D1")

# 1. Import of the "Bug829_resu_mode.med" file at first time
med_file_path = datadir + "Bug829_resu_mode.med"
OpenDataFile(med_file_path)
med_reader1 = pvsimple.GetActiveSource()
if med_reader1 is None:
    raise RuntimeError, "Bug829_resu_mode.med was not imported!!!"

# 2. Creation of a set of "DeformedShape and ScalarMap" presentations, based on time stamps of "MODES_DEPL" field
errors=0
sizes=[]

for i in range(1,11):
    presentation = DeformedShapeAndScalarMapOnField(med_reader1, EntityType.NODE, "MODES___DEPL____________________", i)
    if presentation is None : 
        raise RuntimeError, "Presentation is None!!!"

    pic_path = os.path.join(picturedir, "npal19999_1_time_stamp_" + str(i) + "." + pictureext)
    process_prs_for_test(presentation, pvsimple.GetRenderView(), pic_path)

    sizes.append(os.path.getsize(pic_path))

if abs(max(sizes)-min(sizes)) > 0.01*max(sizes):
    print "WARNING!!! Pictures have different sizes!!!"
    errors += 1
    for i in range(1,11):
	picture_name = "npal19999_1_time_stamp_" + str(i) + "." + pictureext
	print "Picture: " + picture_name + "; size: " + str(sizes[i-1]) 
    raise RuntimeError

# 3. Import of the "Bug829_resu_mode.med" file at second time
OpenDataFile(med_file_path)
med_reader2 = pvsimple.GetActiveSource()
if med_reader2 is None:
    raise RuntimeError, "Bug829_resu_mode.med was not imported second time!!!"

# 4. Creation of a set of "DeformedShape and ScalarMap" presentations, based on time stamps of "MODES_DEPL" field
errors = 0
sizes=[]

for i in range(1,11):
    presentation = DeformedShapeAndScalarMapOnField(med_reader2, EntityType.NODE, "MODES___DEPL____________________", 11-i)
    if presentation is None : 
        raise RuntimeError, "Presentation is None!!!"

    pic_path = os.path.join(picturedir, "npal19999_2_time_stamp_" + str(i) + "." + pictureext)
    process_prs_for_test(presentation, pvsimple.GetRenderView(), pic_path)

    sizes.append(os.path.getsize(pic_path))

if abs(max(sizes)-min(sizes)) > 0.01*max(sizes):
    print "WARNING!!! Pictures have different sizes!!!"
    errors += 1
    for i in range(1,11):
	picture_name = "npal19999_2_time_stamp_" + str(i) + "." + pictureext
	print "Picture: " + picture_name + "; size: " + str(sizes[i-1]) 
    raise RuntimeError
