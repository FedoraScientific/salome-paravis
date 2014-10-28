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

# This case corresponds to: /visu/SWIG_scripts/A3 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple


my_paravis = paravis.myParavis

print 'Importing "TimeStamps.med"...............',
file_path = datadir + "TimeStamps.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

print "Creating Cut Lines........................",
med_field = "vitesse"
cutlines = CutLinesOnField(med_reader, EntityType.NODE, med_field, 1,
                           nb_lines=20,
                           orientation1=Orientation.XY,
                           orientation2=Orientation.ZX)
if cutlines is None:
    print "FAILED"
else:
    print "OK"

print "Getting a viewer.........................",
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    reset_view(view)
    print "OK"

cutlines.Visibility = 1
view.ResetCamera()
pvsimple.Render()

print "Creating an Animation.....................",
scalarmap = ScalarMapOnField(med_reader, EntityType.NODE, med_field, 2)

scene = pvsimple.AnimateReader(med_reader, view)
if scene is None:
    print "FAILED"
else:
    print "OK"
