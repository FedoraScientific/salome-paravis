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

# This case corresponds to: /visu/SWIG_scripts/A7 case

import time

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple


my_paravis = paravis.myParavis
view = pvsimple.GetRenderView()
reset_view(view)
delay = 1


def after_set(error_string, prop_name, old_value, new_value):
    print prop_name, ": old_value = ", old_value, "new_value  = ", new_value
    if error_string == "":
        time.sleep(delay)
    else:
        print prop_name, " error = ", error_string
        pass
    pass


def change_representation(prs, repres, opacity, linew):
    if prs is None:
        print "FAILED"
        return
    else:
        print "OK"

    display_only(prs, view)
    view.ResetCamera()

    time.sleep(delay)

    old_prs_type = prs.Representation
    if old_prs_type != repres:
        err_str = ''
        try:
            prs.Representation = repres
        except:
            err_str = "Error"
        after_set(err_str, "Representation", old_prs_type, repres)
        pass

    old_opacity = prs.Opacity
    if old_opacity != opacity:
        err_str = ''
        try:
            prs.Opacity = opacity  # double value [0, 1]
        except:
            err_str = "Error"
        after_set(err_str, "Opacity", old_opacity, opacity)
        pass

    old_linew = prs.LineWidth
    if old_linew != linew:
        err_str = ''
        try:
            prs.LineWidth = linew  # double value, recommended round [1, 10]
        except:
            err_str = "Error"
        after_set(err_str, "LineWidth", old_linew, linew)
        pass

    print ""
    pass


# ResOK_0000.med

print 'Import "ResOK_0000.med"...............',
file_path = datadir + "ResOK_0000.med"
field_name = 'vitesse'
node_entity = EntityType.NODE

OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

print "Creating Scalar Map.......",
scmap = ScalarMapOnField(med_reader, node_entity, field_name, 1)
change_representation(scmap, 'Surface With Edges', 0.3, 5)

print "Creating Stream Lines.......",
slines = StreamLinesOnField(med_reader, node_entity, field_name, 1)
change_representation(slines, 'Wireframe', 0.5, 3)

print "Creating Vectors..........",
vec = VectorsOnField(med_reader, node_entity, field_name, 1)
change_representation(vec, 'Wireframe', 0.7, 2)

print "Creating Iso Surfaces.....",
iso = IsoSurfacesOnField(med_reader, node_entity, field_name, 1)
change_representation(iso, 'Surface', 0.4, 8)

print "Creating Cut Planes.......",
clines = CutPlanesOnField(med_reader, node_entity, field_name, 1)
change_representation(clines, 'Points', 0.6, 4)


# fra.med

print 'Import "fra.med"...............',
file_path = datadir + "fra.med"
field_name = 'VITESSE'

pvsimple.SetActiveSource(None)
OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

print "Creating Scalar Map.......",
scmap = ScalarMapOnField(med_reader, node_entity, field_name, 1)
change_representation(scmap, 'Wireframe', 0.5, 3)

print "Creating Iso Surfaces.....",
iso = IsoSurfacesOnField(med_reader, node_entity, field_name, 1)
change_representation(iso, 'Wireframe', 0.5, 3)

print "Creating Cut Planes.......",
cplanes = CutPlanesOnField(med_reader, node_entity, field_name, 1)
change_representation(cplanes, 'Surface', 0.5, 3)

print "Creating Scalar Map On Deformed Shape.......",
defmap = DeformedShapeAndScalarMapOnField(med_reader, node_entity,
                                          field_name, 1)
change_representation(defmap, 'Surface', 0.5, 3)

print "Creating Deformed Shape.......",
defshape = DeformedShapeOnField(med_reader, node_entity, field_name, 1)
defshape.ColorArrayName = field_name
change_representation(defshape, 'Wireframe', 0.5, 3)

print "Creating Cut Lines.......",
clines = CutLinesOnField(med_reader, node_entity, field_name, 1)
change_representation(clines, 'Wireframe', 0.5, 3)

print "Creating Plot 3D.......",
plot3d = Plot3DOnField(med_reader, node_entity, field_name, 1)
change_representation(plot3d, 'Surface', 0.5, 3)
change_representation(plot3d, 'Outline', 0.8, 2)
