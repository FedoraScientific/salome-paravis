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

# This case corresponds to: /visu/3D_viewer/A7 case
# Create 3D Viewer and test set view properties for CutPlanes presentation
# Author: POLYANSKIKH VERA
from paravistest import *
from presentations import *
from pvsimple import *
import sys
import paravis
import time

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/A7")

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep
#import file
my_paravis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

file_name = datadir + "Tetra4.med"
print " --------------------------------- "
print "file ", file_name
print " --------------------------------- "

my_paravis.ImportFile(file_name)
proxy = GetActiveSource()
if proxy is None:
    raise RuntimeError("Error: can't import file.")
else:
    print "OK"

represents = [RepresentationType.POINTS, RepresentationType.WIREFRAME,\
RepresentationType.SURFACE, RepresentationType.VOLUME]
shrinks = [0, 1]
shadings = [0, 1]
opacities = [1.0, 0.5, 0.0]
linewidths = [1.0, 3.0, 10.0]
compare_prec = 0.00001

field_name = 'scalar_field'

print "\nCreating cut planes.......",
cut_planes = CutPlanesOnField(proxy, EntityType.CELL, field_name, 1,
nb_planes=10, orientation=Orientation.YZ, displacement=0.5)
if cut_planes is None:
    raise RuntimeError("Error!!! Presentation wasn't created...")

display_only(cut_planes, my_view)
reset_view(my_view)
Render(my_view)

print "\nChange Presentation Parameters..."

for reprCode in represents:
    repr = RepresentationType.get_name(reprCode)
    if reprCode == RepresentationType.VOLUME:
        is_good = call_and_check(cut_planes, "Representation", repr, 0)
        if is_good:
            msg = "VOLUME representation must be not available for Iso Surface"
            raise RuntimeError(msg)
    else:
        call_and_check(cut_planes, "Representation", repr, 1)

        for sha in shadings:
            setShaded(my_view, sha)
            call_and_check(cut_planes, "Shading", sha, 1)
            Render(my_view)

            for opa in opacities:
                call_and_check(cut_planes, "Opacity", opa, 1, compare_prec)

                for lwi in linewidths:
                    call_and_check(cut_planes, "LineWidth", lwi, 1,
                    compare_prec)
                    time.sleep(1)

                    # save picture in file
                    # Construct image file name
                    pic_name = picturedir + "params_" + repr.replace(' ', '_') + "_any_" +  \
                        str(sha) + "_" + str(opa) + "_" + str(lwi) + "." + pictureext
                    # Show and record the presentation
                    process_prs_for_test(cut_planes, my_view, pic_name)
