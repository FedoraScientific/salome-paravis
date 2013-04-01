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

# This case corresponds to: /visu/StreamLines/F9 case

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import StreamLinesOnField, EntityType, \
     reset_view, process_prs_for_test
import paravis
import pvsimple


my_paravis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "StreamLines/F9")

# Import of the "Bug829_resu_mode.med" file
file_path = datadir + "Bug829_resu_mode.med"

my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError("Bug829_resu_mode.med was not imported!!!")

# Get view
view = pvsimple.GetRenderView()

# Create a set of Stream Lines, based on time stamps of "MODES_DEPL" field
print "BREAKPOINT_1"

for i in range(1, 11):
    prs = StreamLinesOnField(med_reader, EntityType.NODE, "MODES_DEPL", i)
    if prs is None:
        raise RuntimeError("Presentation on timestamp {0} is None!!!".
                           format(i))

    picture_path = picturedir + "/" + "time_stamp_" + str(i) + "." + pictureext
    process_prs_for_test(prs, view, picture_path)

print "BREAKPOINT_2"
