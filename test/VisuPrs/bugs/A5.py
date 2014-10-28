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

# This case corresponds to: /visu/bugs/A5 case

import os
import salome

from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
import pvserver as paravis
import pvsimple

myStudy = salome.myStudy
myStudyManager = salome.myStudyManager

data_file = datadir + "TimeStamps.med"

tmp_dir = os.getenv("TmpDir")
if tmp_dir == None:
    tmp_dir = "/tmp"

save_file = tmp_dir + "/TimeStamps_save.hdf"
print "Save to file ", save_file

paravis.myParavis.ImportFile(data_file)
myStudyManager.SaveAs(save_file, myStudy, 0)
myStudyManager.Close(myStudy)
openedStudy = myStudyManager.Open(data_file)
myStudyManager.Close(openedStudy)

os.remove(save_file)

print "OK"
