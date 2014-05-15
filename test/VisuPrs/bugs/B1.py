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

# This case corresponds to: /visu/bugs1/B1 case


import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis
os.environ["PARAVIS_TEST_PICS"] = sys.argv[1]
picturedir = get_picture_dir("bugs/B1")

# 1. Import MED file
med_file_path = datadir + "resultat.01.med"

print 'Importing "resultat.01.med"....',
my_paravis.ImportFile(med_file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError, "resultat.01.med was not imported!!!"
else:
    print "OK"

# 2. Creation of presentation of each group
extGrp = pvs.ExtractGroup()
extGrp.UpdatePipelineInformation()
groups = get_group_names(extGrp)

errors = 0
i = 0
for group in groups:
    i += 1
    shor_name = group.split('/')[-1]
    print "group: ", shor_name
    prs = MeshOnGroup(med_reader, extGrp, group)
    
    if prs is None : 
        print "FAILED!!! Created presentation is None!!!"
        errors += 1
    else :
        print "Presentation was created!"
        pic_path = os.path.join(picturedir, shor_name.strip().split("_")[0]+str(i)+"."+pictureext)
        process_prs_for_test(prs, pvsimple.GetRenderView(), pic_path)                    
        

# check errors
if errors == 1:
    raise RuntimeError, "There is an error was occured... For more info see ERROR message above.."
elif errors > 1:
    raise RuntimeError, "There are some errors were occured... For more info see ERRORs messages above.."

