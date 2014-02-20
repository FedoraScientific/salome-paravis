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

# This case corresponds to: /visu/SWIG_scripts/A1 case

from paravistest import datadir
from presentations import *
import paravis
import pvsimple


my_paravis = paravis.myParavis

#====================Stage1: Importing MED file====================

print "**** Stage1: Importing MED file"

print 'Import "ResOK_0000.med"...............',
file_path = datadir + "ResOK_0000.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

#====================Stage2: Displaying vector field===============

print "**** Stage3: Displaying vector field"

print 'Get view...................',
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    reset_view(view)
    print "OK"

print "Creating Scalar Map.......",
scalarmap = ScalarMapOnField(med_reader, node_entity, 'vitesse', 1)
if scalarmap is None:
    print "FAILED"
else:
    bar = get_bar()
    bar.Orientation = 'Horizontal'
    bar.Position = [0.1, 0.1]
    bar.Position2 = [0.1, 0.25]
    bar.AspectRatio = 3

    display_only(scalarmap, view)
    print "OK"

view.ResetCamera()

print "Creating Stream Lines.....",
streamlines = StreamLinesOnField(med_reader, node_entity, 'vitesse', 1)
if streamlines is None:
    print "FAILED"
else:
    display_only(streamlines, view)
    print "OK"

print "Creating Vectors..........",
vectors = VectorsOnField(med_reader, node_entity, 'vitesse', 1)
if vectors is None:
    print "FAILED"
else:
    display_only(vectors, view)
    print "OK"

print "Creating Iso Surfaces.....",
isosurfaces = IsoSurfacesOnField(med_reader, node_entity, 'vitesse', 1)
if isosurfaces is None:
    print "FAILED"
else:
    display_only(isosurfaces, view)
    print "OK"

print "Creating Cut Planes.......",
cutplanes = CutPlanesOnField(med_reader, node_entity, 'vitesse', 1,
                             nb_planes=30, orientation=Orientation.YZ)
if cutplanes is None:
    print "FAILED"
else:
    display_only(cutplanes, view)
    print "OK"

print "Creating Scalar Map On Deformed Shape.......",
scalarmapondefshape = DeformedShapeAndScalarMapOnField(med_reader,
                                                       node_entity,
                                                       'vitesse', 2,
                                                       None,
                                                       cell_entity,
                                                       'pression')
if scalarmapondefshape is None:
    print "FAILED"
else:
    display_only(scalarmapondefshape, view)
    print "OK"

#====================Stage3: Another Med file import====================

print 'Import "Fields_group3D.med"...............',
file_path = datadir + "Fields_group3D.med"
my_paravis.ImportFile(file_path)
med_reader1 = pvsimple.GetActiveSource()

if med_reader1 is None:
    print "FAILED"
else:
    print "OK"

#====================Stage4: Displaying scalar field====================

print "**** Stage4: Displaying scalar field"

print "Creating Scalar Map.......",
scalarmap1 = ScalarMapOnField(med_reader1, cell_entity, 'scalar_field', 1)
if scalarmap1 is None:
    print "FAILED"
else:
    display_only(scalarmap1, view)
    print "OK"

view.ResetCamera()

print "Creating Iso Surfaces.....",
isosurfaces1 = IsoSurfacesOnField(med_reader1, cell_entity, 'scalar_field', 1)
if isosurfaces1 is None:
    print "FAILED"
else:
    display_only(isosurfaces1, view)
    print "OK"

print "Creating Cut Planes.......",
cutplanes1 = CutPlanesOnField(med_reader1, cell_entity, 'scalar_field', 1,
                              orientation=Orientation.YZ)
if cutplanes1 is None:
    print "FAILED"
else:
    print "OK"

slice_filter = pvsimple.GetActiveSource()
slice_filter.SliceType.Normal = [1.0, 0.0, 0.0]
display_only(cutplanes1, view)

print "Creating Scalar Map On Deformed Shape.......",
scalarmapondefshape1 = DeformedShapeAndScalarMapOnField(med_reader1,
                                                        cell_entity,
                                                        'vectoriel_field', 1)
if scalarmapondefshape1 is None:
    print "FAILED"
else:
    display_only(scalarmapondefshape1, view)
    print "OK"

#====================Stage5: Object browser popup====================

print "**** Stage5: Object browser popup"

print "Creating mesh.............",

mesh_name = 'mailles_MED'
mesh = MeshOnEntity(med_reader1, mesh_name, cell_entity)
if mesh is None:
    print "FAILED"
else:
    display_only(mesh, view)
    print "OK"

print "Changing type of presentation of mesh:"
mesh.Representation = 'Wireframe'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

mesh.Representation = 'Points'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

mesh.Representation = 'Surface'
pvsimple.Render()
prs_type = mesh.Representation
print "Presentation type..", prs_type

shrink = pvsimple.Shrink(med_reader1)
mesh_shrinked = pvsimple.GetRepresentation(shrink)
display_only(mesh_shrinked, view)

print "Changing color of mesh....",
color = [0, 0, 1]
mesh.DiffuseColor = color
display_only(mesh, view)
print "OK"
