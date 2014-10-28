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

# This case corresponds to: /visu/SWIG_scripts/B1 case

import math
import random
from time import sleep

from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple


DELAY = 0.25

my_paravis = paravis.myParavis

# Get view
view = pvsimple.GetRenderView()
sleep(DELAY)

# Destroy the view
pvsimple.Delete(view)


# Create view and set background
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0, 0.3, 1]
view.Background = color
pvsimple.Render()

# Import MED file
med_file = datadir + "pointe.med"
field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1

my_paravis.ImportFile(med_file)
med_reader = pvsimple.GetActiveSource()


# Create scalar map
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
print "ScalarMapOnField(...)"
pvsimple.Show(scalarmap.Input)

# Set view properties
print "view.CameraFocalPoint = [0, 0, 0]"
view.CameraFocalPoint = [0, 0, 0]
print "view.CameraParallelScale = 2"
view.CameraParallelScale = 2
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)

# Play with scalar bar
bar = get_bar()
lt = bar.LookupTable

range_min = lt.RGBPoints[0]
range_max = lt.RGBPoints[4]
delta = (range_max - range_min) / 2.0
nb_colors = lt.NumberOfTableValues
nb_colors = 64
lt.Discretize = 1
for i in xrange(2, nb_colors):
    lt.NumberOfTableValues = nb_colors
    x = range_min + delta * i / nb_colors
    y = range_max - delta * i / nb_colors
    lt.RGBPoints[0] = x
    lt.RGBPoints[4] = y
    pvsimple.Render(view)
    sleep(DELAY / 4.0)

lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max

print "pvsimple.ResetCamera(view)"
pvsimple.Render(view)
sleep(DELAY)

# Destroy the view
print "Destroy the view with Scalar Map"
pvsimple.Delete(view)


# Create another view for cut planes
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0, 0.7, 0]
view.Background = color
pvsimple.Render(view)

displacement = 0.5
orient = Orientation.YZ
cutplanes = CutPlanesOnField(med_reader, entity, field_name, timestamp,
                             orientation=orient,
                             displacement=displacement)
print "CutPlanesOnField(...)"

display_only(cutplanes, view)
print "display_only(cutplanes, view)"

cam_pos = view.CameraPosition
cam_pos[0] = cam_pos[0] + 10
print "Set view.CameraPosition"
cutplanes.Scale[0] = 3
cutplanes.Scale[1] = 10
pvsimple.Render(view)
sleep(DELAY)

pvsimple.ResetCamera(view)

slice_filter = cutplanes.Input
offset_vals = slice_filter.SliceOffsetValues
nb_planes = len(offset_vals)
nb_planes = 30
bounds = get_bounds(med_reader)
for i in xrange(nb_planes, 1, -1):
    x = math.pi / 2.0 * (nb_planes - i) / nb_planes
    y = math.pi / 2.0 * (nb_planes - i) / nb_planes
    normal = get_normal_by_orientation(orient, x, y)
    slice_filter.SliceType.Normal = normal
    pos = get_positions(i, normal, bounds, displacement)
    slice_filter.SliceOffsetValues = pos
    pvsimple.Render(view)
    sleep(DELAY)

nb_planes = 10
normal = [0, 1, 0]
slice_filter.SliceType.Normal = normal
for i in xrange(1, nb_planes):
    pos = get_positions(i, normal, bounds, displacement)
    slice_filter.SliceOffsetValues = pos
    pvsimple.Render(view)
    sleep(DELAY)

slice_filter.SliceType.Normal = [0, 0, 1]
slice_filter.UpdatePipeline()
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)
sleep(DELAY)

# Destroy the view
print "Destroy the view with Cut Planes"
pvsimple.Delete(view)


# Create one more view for isosurfaces
view = pvsimple.CreateRenderView()
reset_view(view)

color = [1, 0.7, 0]
view.Background = color
pvsimple.Render(view)
sleep(DELAY)

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
print "IsoSurfacesOnField(...)"
display_only(isosurf, view)
pvsimple.ResetCamera(view)
print "display_only(isosurf, view)"
sleep(DELAY)

contour = isosurf.Input
nb_surfaces = len(contour.Isosurfaces)
nb_surfaces = 32
scalar_range = get_data_range(med_reader, entity, field_name, cut_off=True)
for i in xrange(2, nb_surfaces):
    contours = get_contours(scalar_range, i)
    contour.Isosurfaces = contours
    pvsimple.Render(view)
    sleep(DELAY)

contour.Isosurfaces = get_contours(scalar_range, 10)
contour.UpdatePipeline()
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)
sleep(DELAY)

# Destroy the view
print "Destroy the view with Iso Surfaces"
pvsimple.Delete(view)


# Create one more view for cut lines
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0.7, 0.7, 0.7]
view.Background = color
pvsimple.Render(view)
sleep(DELAY)

cutlines, curves = CutLinesOnField(med_reader, entity, field_name, timestamp,
                                   orientation1=Orientation.ZX,
                                   orientation2=Orientation.YZ,
                                   generate_curves=True)
print "CutLinesOnField(...)"
display_only(cutlines, view)
pvsimple.ResetCamera(view)
print "display_only(cutlines, view)"
sleep(DELAY)

xy_view = pvsimple.CreateXYPlotView()
print "pvsimple.CreateXYPlotView()"
index = 0
for curve in curves:
    xyrep = pvsimple.Show(curve, xy_view)
    xyrep.AttributeType = 'Point Data'
    xyrep.UseIndexForXAxis = 0
    xyrep.XArrayName = 'arc_length'
    pvsimple.Render(xy_view)
    set_visible_lines(xyrep, [field_name])
    xyrep.SeriesLabel = [field_name, 'Y' + str(index)]
    r = str(random.random())
    g = str(random.random())
    b = str(random.random())
    xyrep.SeriesColor = [field_name, r, g, b]
    index += 1

pvsimple.Render(xy_view)
sleep(DELAY)

# Destroy two views
#print "Destroy two views: with Cut Lines and the curves"
#pvsimple.Delete(view)
#pvsimple.Delete(xy_view)


# Create one more view for cut segment
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0.0, 0.7, 0.3]
view.Background = color
pvsimple.Render(view)
sleep(DELAY)

point1 = [0.0, -1.0, 2.5]
point2 = [0.0, 1.0, 2.5]
cutsegment = CutSegmentOnField(med_reader, entity, field_name, timestamp,
                               point1, point2)

print "CutSegmentOnField(...)"
display_only(cutsegment, view)
pvsimple.ResetCamera(view)
print "display_only(cutsegment, view)"
sleep(DELAY)

xy_view = pvsimple.CreateXYPlotView()
curve = pvsimple.Show(cutsegment.Input, xy_view)
curve.AttributeType = 'Point Data'
curve.UseIndexForXAxis = 0
curve.XArrayName = 'arc_length'
set_visible_lines(curve, [field_name])

pvsimple.Render(xy_view)
sleep(DELAY)


# Create one more view for animation
view = pvsimple.CreateRenderView()
reset_view(view)

med_file = datadir + "TimeStamps.med"
field_name = "vitesse"
entity = EntityType.NODE
timestamp = 2

my_paravis.ImportFile(med_file)
med_reader = pvsimple.GetActiveSource()

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
print "IsoSurfacesOnField(...)"
pvsimple.ResetCamera(view)

print "Start Animation"
pvsimple.AnimateReader(med_reader, view)
