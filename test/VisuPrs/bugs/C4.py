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

# This case corresponds to: /visu/bugs2/C4 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/C4")

# 1. Import MED file
med_file_path = datadir + "forma01f.resu.med"

print 'Importing "forma01f.resu.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Creation of ScalarMap On DeformedShape presentation
scales=[None, 0, 1e-05]
fields=["RESU1_DEPL", "RESU1_SIGM_ELNO_DEPL"]
entities=[EntityType.NODE, EntityType.CELL]
entities_str=["NODE", "CELL"]

view = pvsimple.GetRenderView()

for scale in scales:
    for i in range(len(fields)):
	print "Field: ", fields[i], "; Scale: ", scale
        presentation = None
        try:
            presentation = DeformedShapeAndScalarMapOnField(med_reader, entities[i], fields[i], 1)
        except ValueError as e:
            print "Error:", e
        
	if presentation is not None:
	    if scale is not None:
	        presentation.Input.ScaleFactor = scale
            # save picture
            pic_path = os.path.join(picturedir, "MAIL_" + entities_str[i] + "_" + fields[i] + "_" + str(scale) + "_." + pictureext)
            process_prs_for_test(presentation, view, pic_path)
        else:
	    print "FAILED! Created presentation is None!!!"

