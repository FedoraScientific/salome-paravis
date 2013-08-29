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

# This case corresponds to: /visu/dump_study/B5 case

import os
import sys
import salome

class SalomeSession(object):
    def __init__(self):
        import runSalome
        import sys
        sys.argv += ["--show-desktop=1"]
        sys.argv += ["--splash=0"]
        sys.argv += ["--modules=MED,VISU,PARAVIS"]
        clt, d = runSalome.main()
        port = d['port']
        self.port = port
        return
    pass


# 1. Opening saved study

# run Salome
salome_session = SalomeSession()
salome.salome_init()

file_name = os.path.join(os.path.dirname(sys.argv[0]), "test1.hdf")
opened_study = salome.myStudyManager.Open(file_name)
salome.myStudy = opened_study

# 2. Load PARAVIS module
import paravis

# 3. Find IsoSurfaces
from pvsimple import *
obj = FindSource('IsoSurfaces')
if obj is None:
    print "FAILED: can't find IsoSurfaces!!!"

# 4. Remove med reader object and all other sources
for obj in GetSources().values():
    Delete(obj)
    
# 5. Check results
obj = FindSource('ScalarMap')
if obj is not None:
    print "FAILED: ScalarMap was not deleted!!!"
