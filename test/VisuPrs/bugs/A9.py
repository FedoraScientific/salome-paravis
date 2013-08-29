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

# This case corresponds to: /visu/bugs/A9 case

import os
import sys

from paravistest import datadir, get_picture_dir, pictureext
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "bugs/A9")

# 1. Step1: Import MED file
print "**** Step1: Importing MED file"

print 'Import "sortie_med_volumique.med"...............',
file_path = datadir + "sortie_med_volumique_v3.0.6.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

print 'Get view........................................',
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    reset_view(view)
    print "OK"
    
mesh_name = 'Volume_fluide'
cell_entity = EntityType.CELL
node_entity = EntityType.NODE

# 2. Step2: Displaying mesh
errors = 0

print "**** Step2: Display mesh"
print "BREAKPOINT_1"

# Creation of Mesh presentation on nodes
print "Creating Mesh presentation on nodes......."
mesh = MeshOnEntity(med_reader, mesh_name, node_entity)
if mesh is None:
    print "ERROR!!! Mesh presentation on nodes creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "MeshPresentation_OnNodes." + pictureext)
    process_prs_for_test(mesh, view, picture_path)
    print "OK"

# Creation of Mesh presentation on cells
print "Creating Mesh presentation on cells......."
mesh = MeshOnEntity(med_reader, mesh_name, cell_entity)
if mesh is None:
    print "ERROR!!! Mesh presentation on cells creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "MeshPresentation_OnCells." + pictureext)
    process_prs_for_test(mesh, view, picture_path)
    print "OK"

# 3. Step3: Displaying scalar field 'Dissip'
print "**** Step3: Display scalar field 'Dissip'"

entity = cell_entity

# Scalar Map creation
print "Creating Scalar Map.......",
scalarmap = ScalarMapOnField(med_reader, entity, 'Dissip', 1)

if scalarmap is None:
    print "ERROR!!! Scalar Map creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "ScalarMap_Dissip." + pictureext)
    process_prs_for_test(scalarmap, view, picture_path)
    print "OK"

# Iso Surfaces creation
print "Creating Iso Surfaces.......",
isosurfaces = IsoSurfacesOnField(med_reader, entity, 'Dissip', 1)

if isosurfaces is None:
    print "ERROR!!! Iso Surfaces creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "IsoSurfaces_Dissip." + pictureext)
    process_prs_for_test(isosurfaces, view, picture_path)
    print "OK"

# Gauss Points creation
print "Creating Gauss Points.......",
gausspoints = GaussPointsOnField(med_reader, entity, 'Dissip', 1)

if gausspoints is None:
    print "ERROR!!! Gauss Points creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "GaussPoints_Dissip." + pictureext)
    process_prs_for_test(isosurfaces, view, picture_path)
    print "OK"

# 4. Step4: Displaying vectoriel field 'VitesseX'
entity = cell_entity

print "**** Step5: Display vectoriel field 'VitesseX'"
# Deformed Shape creation
print "Creating Deformed Shape.......",

defshape = DeformedShapeOnField(med_reader, entity, 'VitesseX', 1)
if defshape is None:
    print "ERROR!!! Deformed Shape creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "DeformedShape_VitesseX." + pictureext)
    process_prs_for_test(defshape, view, picture_path)
    print "OK"

# Vectors creation
print "Creating Vectors.......",
vectors = VectorsOnField(med_reader, entity, 'VitesseX', 1)

if vectors is None:
    print "ERROR!!! Vectors creation FAILED!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "Vectors_VitesseX." + pictureext)
    process_prs_for_test(vectors, view, picture_path)
    print "OK"

# Scalar Map On Deformed Shape creation
print "Creating Scalar Map On Deformed Shape.......",
smapondefshape = DeformedShapeAndScalarMapOnField(med_reader, entity, 'VitesseX', 1)

if smapondefshape is None:
    print "ERROR!!! ScalarMapOnDeformedShape creation failed!!!"
    errors += 1
else:
    picture_path = os.path.join(picturedir, "ScalMapOnDefShape_VitesseX." + pictureext)
    process_prs_for_test(smapondefshape, view, picture_path)
    print "OK"

if errors == 1:
    raise RuntimeError, "There is an error was occured... For more info see ERROR message above.."
elif errors > 1:
    raise RuntimeError, "There are some errors were occured... For more info see ERRORs messages above.."
print "BREAKPOINT_2"
