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

# This case corresponds to: /visu/imps/A6 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis
error =0

# Import MED file
file_path = datadir + "Bug891_Box.resu.med"
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "Error!!! med file is not imported"
    error = error+1

# Create Scalar Map
scalarmap = ScalarMapOnField(med_reader, EntityType.NODE, 'RESUMECAEQUI_ELGA_SIGM__________', 1)
if scalarmap is None:
    print "Error!!! ScalarMap is not created"
    error = error+1
    
scalarmap.Visibility = 1
reset_view()

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."

