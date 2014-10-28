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

# This case corresponds to: /visu/dump_study/B0 case

from paravistest import datadir
from presentations import *
import pvserver as paravis
from pvsimple import *

my_paravis = paravis.myParavis

# 1. TimeStamps.med import
file_path = datadir + "TimeStamps.med"
my_paravis.ImportFile(file_path)
med_reader = GetActiveSource()
if med_reader is None :
    raise RuntimeError, "TimeStamps.med wasn't imported..."

# 2. Presentations creation
errors = 0
prs_names = ["ScalarMap", "IsoSurfaces", "CutPlanes", "CutLines", "DeformedShape", "Vectors", "StreamLines", "Plot3D", "DeformedShapeAndScalarMap", "GaussPoints"]
prs_list = []

med_field = "vitesse"

for name in prs_names:
    print "Creation of ", name, " presentation..."
    if name == "GaussPoints":
        prs = GaussPointsOnField(med_reader, EntityType.CELL, "pression", 1)  
 	pass
    else: 
	prs = eval(name + "OnField(med_reader, EntityType.NODE, med_field, 1)")
    if prs is None:
        print "ERROR!!! ", name," presentation wasn't created..."
        errors += 1
    else:
        RenameSource(name, prs.Input)
        prs_list.append(prs)

# 3. Dump Study
path_to_save = os.path.join(os.getenv("HOME"), "AllPresentations.py")
SaveTrace( path_to_save )

# 4. Delete the created objects, recreate the view
source_list = GetSources().values()
for source in source_list:
    Delete(source)

Delete(GetActiveView())
view = CreateRenderView()

# 5. Execution of the created script
execfile(path_to_save)

# 6. Check the restored objects
for name in prs_names:
    source = FindSource(name)
    if source is None:
        print "There is no ", name, " in the study (must be created by executed python script)!!!"
        errors += 1
    else:
        print name + " was found..."

if errors > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."


