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

# This case corresponds to: /visu/united/A4 case

from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

# 1. Import MED file
print "**** Step1: Import MED file"

print 'Import "Hexa8.med"....................',
file_path = datadir + "Hexa8.med"
my_paravis.ImportFile(file_path)
med_reader1 = pvsimple.GetActiveSource()

if med_reader1 is None:
    print "FAILED"
else:
    print "OK"

mesh_name1 = 'Maillage_MED_HEXA8'
scalar_field = 'scalar_field'
vectoriel_field = 'vectoriel_field'
view = pvsimple.GetRenderView()

print "Creating Scalar Map...................",
scalarmap1 = ScalarMapOnField(med_reader1, cell_entity, scalar_field, 1)
if scalarmap1 is None:
    print "FAILED"
else:
    display_only(scalarmap1, view)
    reset_view(view)
    print "OK"
    
print "Creating Vectors......................",
vectors1 = VectorsOnField(med_reader1, cell_entity, vectoriel_field, 1)
if vectors1 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Deformed Shape...............",
defshape1 = DeformedShapeOnField(med_reader1, cell_entity, vectoriel_field, 1)
if defshape1 is None:
    print "FAILED"
else:
    print "OK"

print "Creating mesh.........................",
mesh1 = MeshOnEntity(med_reader1, mesh_name1, cell_entity)
if mesh1 is None:
    print "FAILED"
else:
    mesh1.Representation = 'Wireframe'
    display_only(mesh1, view)
    reset_view(view)
    print "OK"

print "Displaying vectors....................",
display_only(vectors1, view)
print "OK"

print "Displaying Deformed Shape.............",
display_only(defshape1, view)
print "OK"

print "Set scale factor of  Deformed Shape...",
warp_vector = defshape1.Input
warp_vector.ScaleFactor = 10
print "OK"

print "Displaying changed Deformed Shape.....",
display_only(defshape1, view)
print "OK"

print "Editing Vectors.......................",
glyph = vectors1.Input
glyph.SetScaleFactor = 2
vectors1.LineWidth = 2
glyph.GlyphType.Center = [0.5, 0.0, 0.0] # TAIL position
print "OK"

print "Displaying changed Vectors............",
vectors1.Visibility = 1
pvsimple.Render()
print "OK"

hide_all(view)

# 2. Second MED file import

print 'Importing file "cube_hexa8_quad4.med".',
file_path = datadir + "cube_hexa8_quad4.med"
mesh_name2 = 'CUBE_EN_HEXA8_QUAD4'
my_paravis.ImportFile(file_path)
med_reader2 = pvsimple.GetActiveSource()

if med_reader2 is None:
    print "FAILED"
else:
    print "OK"

print "Creating mesh.........................",
mesh2 = MeshOnEntity(med_reader2, mesh_name2, cell_entity)
if mesh2 is None:
    print "FAILED"
else:
    display_only(mesh2, view)
    print "OK"

print "Setting wireframe repr. of mesh.......",
mesh2.Representation = 'Wireframe'
print "OK"

print "Creating Vectors......................",
vectors2 = VectorsOnField(med_reader2, cell_entity, "fieldcelldouble", 1)
if vectors1 is None:
    print "FAILED"
else:
    glyph2 = vectors2.Input
    glyph2.SetScaleFactor = 0.2
    vectors2.LineWidth = 2
    glyph2.GlyphType.Center = [0.5, 0.0, 0.0] # TAIL position
    print "OK"



print "Displaying Vectors (with mesh)........",
vectors2.Visibility = 1
pvsimple.Render()
print "OK"

hide_all(view)

# 3. Third MED file import

print 'Importing file "Penta6.med"...........',
file_path = datadir + "Penta6.med"
my_paravis.ImportFile(file_path)
med_reader3 = pvsimple.GetActiveSource()

if med_reader3 is None:
    print "FAILED"
else:
    print "OK"
    
print "Creating Cut Planes...................",
cutplanes1 = CutPlanesOnField(med_reader3, cell_entity, scalar_field, 1)
if cutplanes1 is None:
    print "FAILED"
else:
    display_only(cutplanes1, view)
    reset_view(view)
    print "OK"

print "Creating Deformed Shape...............",
defshape2 = DeformedShapeOnField(med_reader3, cell_entity, vectoriel_field, 1)
if defshape2 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Vectors......................",
vectors3 = VectorsOnField(med_reader3, cell_entity, vectoriel_field, 1)
if vectors3 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Iso Surfaces.....",
isosurfaces1 = IsoSurfacesOnField(med_reader3, cell_entity, vectoriel_field, 1)
if isosurfaces1 is None:
    print "FAILED"
else:
    print "OK"

print "Displaying Vectors (l.w.=2, s.f.=1.5).",
glyph3 = vectors3.Input
glyph3.SetScaleFactor = 1.5
vectors3.LineWidth = 2
display_only(vectors3, view)
print "OK"

print "Displaying Deformed Shape.............",
defshape2.Visibility = 1
pvsimple.Render()
print "OK"

print "Displaying Iso Surfaces...............",
isosurfaces1.Visibility = 1
pvsimple.Render()
print "OK"

hide_all(view)

# 4. Import Quad4,Tetra4 and Tria3 MED files

print 'Importing "Quad4.med".................',
file_path = datadir + "Quad4.med"
my_paravis.ImportFile(file_path)
med_reader4 = pvsimple.GetActiveSource()

if med_reader4 is None:
    print "FAILED"
else:
    print "OK"

mesh_name4 = 'Maillage MED_QUAD4'

print "Creating Iso Surfaces.................",
isosurfaces2 = IsoSurfacesOnField(med_reader4, cell_entity, scalar_field, 1)
if isosurfaces2 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Cut Planes...................",
cutplanes2 = CutPlanesOnField(med_reader4, cell_entity, scalar_field, 1,
                              orientation=Orientation.YZ)
if cutplanes2 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Deformed Shape...............",
defshape3 = DeformedShapeOnField(med_reader4, cell_entity, vectoriel_field, 1, scale_factor=10)
if defshape3 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Vectors......................",
vectors4 = VectorsOnField(med_reader4, cell_entity, vectoriel_field, 1)
if vectors4 is None:
    print "FAILED"
else:
    print "OK"

print "Displaying Iso Surfaces, Def. Shape and Vectors.",
display_only(isosurfaces2, view)
defshape3.Visibility = 1
vectors4.Visibility = 1
pvsimple.Render()
print "OK"

print 'Importing "Tetra4.med"................',
file_path = datadir + "Tetra4.med"
my_paravis.ImportFile(file_path)
med_reader5 = pvsimple.GetActiveSource()

if med_reader5 is None:
    print "FAILED"
else:
    print "OK"

mesh_name5 = 'Maillage MED_TETRA4'

print "Creating Scalar Map...................",
scalarmap2 = ScalarMapOnField(med_reader5, cell_entity, scalar_field, 1)
if scalarmap2 is None:
    print "FAILED"
else:
    print "OK"

print "Creating Vectors......................",
vectors5 = VectorsOnField(med_reader5, cell_entity, vectoriel_field, 1)
if vectors5 is None:
    print "FAILED"
else:
    print "OK"

vectors5.LineWidth = 3
vectors5.Input.SetScaleFactor = 2

print "Displaying Scalar Map and Vectoes.....",
hide_all(view)
display_only(scalarmap2, view)
vectors5.Visibility = 1
reset_view(view)
print "OK"

print 'Importing "Tria3.med".................',
file_path = datadir + "Tria3.med"
my_paravis.ImportFile(file_path)
med_reader6 = pvsimple.GetActiveSource()

if med_reader6 is None:
    print "FAILED"
else:
    print "OK"

mesh_name6 = 'Maillage MED_TRIA3'

print "Creating Scalar Map...................",
scalarmap3 = ScalarMapOnField(med_reader6, cell_entity, scalar_field, 1)
if scalarmap3 is None:
    print "FAILED"
else:
    display_only(scalarmap3, view)
    print "OK"

reset_view(view)

print "Creating Iso Surfaces.................",
isosurfaces3 = IsoSurfacesOnField(med_reader6, cell_entity, scalar_field, 1)
if isosurfaces3 is None:
    print "FAILED"
else:
    display_only(isosurfaces3, view)
    print "OK"

reset_view(view)

print "Creating Deformed Shape...............",
defshape4 = DeformedShapeOnField(med_reader6, cell_entity, vectoriel_field, 1)
if defshape4 is None:
    print "FAILED"
else:
    display_only(defshape4, view)
    print "OK"

reset_view(view)

print "Creating Vectors......................",
vectors6 = VectorsOnField(med_reader6, cell_entity, vectoriel_field, 1)
if vectors6 is None:
    print "FAILED"
else:
    glyph = vectors6.Input
    glyph.GlyphType.Center = [0.5, 0.0, 0.0] # TAIL position
    vectors6.LineWidth = 2
    glyph.SetScaleFactor = 1
    display_only(vectors6, view)
    print "OK"

reset_view(view)

print "Displaying only Def. Shape and Vectors",
display_only(defshape4, view)
vectors6.Visibility = 1
pvsimple.Render()
print "OK"
