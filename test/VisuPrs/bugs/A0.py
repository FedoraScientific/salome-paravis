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

# This case corresponds to: /visu/bugs/A0 case

import sys
import os
from paravistest import datadir, pictureext, get_picture_dir
import presentations
import paravis
import pvsimple

my_paravis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "bugs/A0")

# 1. Import MED file
print 'Importing "hydro_sea_alv.med"...',
file_path = datadir + "hydro_sea_alv.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Create cut lines on "Head" field
mesh_name = "maillage_migr3d"
field_name = "Head"
cell_entity = presentations.EntityType.CELL
timestamps = med_reader.TimestepValues.GetData()

for ts in xrange(1, len(timestamps) + 1):
    print "Timestamp: ", ts
    cutlines = presentations.CutLinesOnField(med_reader, cell_entity, field_name, ts,
                                             orientation1=presentations.Orientation.ZX,
                                             orientation2=presentations.Orientation.YZ)
    pic_name = mesh_name + "_" + str(cell_entity) + "_" + field_name + "_" + str(ts) + "_TCUTLINES." + pictureext
    pic_path = os.path.join(picturedir, pic_name)
    print pic_path
    presentations.process_prs_for_test(cutlines, pvsimple.GetRenderView(), pic_path)
    nb_lines = len(cutlines.Input.SliceOffsetValues)
    print "Number of lines = ", nb_lines
    
    

    




