
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

# This case corresponds to: /visu/StreamLines/G0 case
# StreamTracer filter properties

from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
file_path = datadir + "new_case.rmed"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError, "new_case.rmed was not imported!!!"

# 2. Creation of a set of "StreamLines" presentations, based on time stamps of "RESU_DEPL" field
streamlines = StreamLinesOnField(med_reader, EntityType.NODE, 'RESU_DEPL', 1)
if streamlines is None: 
    raise RuntimeError, "Presentation is None!!!"

import time
time.sleep(10000)

