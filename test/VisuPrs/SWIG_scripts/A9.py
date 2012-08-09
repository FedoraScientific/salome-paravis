# Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

# This case corresponds to: /visu/SWIG_scripts/A9 case
# Import MED file; create Scalar Map, Cut Planes, Cut Lines,
# Cut Segment, Iso Surfaces, Animation; display curves.
 
import math
import random
from time import sleep

from presentations import *


# Delay
DELAY = 0.25

# MED files and table directories
samples_dir = os.getenv("DATA_DIR")
datadir = None
tablesdir = None
if samples_dir is not None:
    samples_dir = os.path.normpath(samples_dir)
    datadir = samples_dir + "/MedFiles/"
    tablesdir = samples_dir + "/Tables/"

# Get view
view = pv.GetRenderView()
sleep(DELAY)

# Destroy the view
pv.Delete(view)


# Create view and set background
view = pv.CreateRenderView()
reset_view(view)

color = [0, 0.3, 1]
view.Background = color
pv.Render()

# Load MED reader plugin
pv_root_dir = os.getenv("PARAVIS_ROOT_DIR")
pv.LoadPlugin(pv_root_dir + "/lib/paraview/libMedReaderPlugin.so")

# Import MED file
med_file = datadir + "pointe.med"
field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1

med_reader = pv.MEDReader(FileName=med_file)
med_reader.UpdatePipeline()

# Create scalar map
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
pv.Show(scalarmap.Input)

# Set view properties
print "view.CameraFocalPoint = [0, 0, 0]"
view.CameraFocalPoint = [0, 0, 0]
print "view.CameraParallelScale = 2"
view.CameraParallelScale = 2
print "pv.ResetCamera(view)"
pv.ResetCamera(view)

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
    pv.Render(view)
    sleep(DELAY / 4.0)

lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max

print "pv.ResetCamera(view)"
pv.ResetCamera(view)

# Create another view for cut planes
view = pv.CreateRenderView()
reset_view(view)

color = [0, 0.7, 0]
view.Background = color
pv.Render(view)

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
pv.Render(view)
sleep(DELAY)

pv.ResetCamera(view)

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
    pv.Render(view)
    sleep(DELAY)

nb_planes = 10
normal = [0, 1, 0]
slice_filter.SliceType.Normal = normal
for i in xrange(1, nb_planes):
    pos = get_positions(i, normal, bounds, displacement)
    slice_filter.SliceOffsetValues = pos
    pv.Render(view)
    sleep(DELAY)

slice_filter.SliceType.Normal = [0, 0, 1]
slice_filter.UpdatePipeline()
print "pv.ResetCamera(view)"
pv.ResetCamera(view)

# Create one more view for iso surfaces
view = pv.CreateRenderView()
reset_view(view)

color = [1, 0.7, 0]
view.Background = color
pv.Render(view)
sleep(DELAY)

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
display_only(isosurf, view)
pv.ResetCamera(view)
print "display_only(isosurf, view)"
sleep(DELAY)

contour = isosurf.Input
nb_surfaces = len(contour.Isosurfaces)
nb_surfaces = 32
scalar_range = get_data_range(med_reader, entity, field_name, cut_off=True)
for i in xrange(2, nb_surfaces):
    contours = get_contours(scalar_range, i)
    contour.Isosurfaces = contours
    pv.Render(view)
    sleep(DELAY)

contour.Isosurfaces = get_contours(scalar_range, 10)
contour.UpdatePipeline()
print "pv.ResetCamera(view)"
pv.ResetCamera(view)

# Create one more view for cut lines
view = pv.CreateRenderView()
reset_view(view)

color = [0.7, 0.7, 0.7]
view.Background = color
pv.Render(view)
sleep(DELAY)

cutlines, curves = CutLinesOnField(med_reader, entity, field_name, timestamp,
                                   orientation1=Orientation.ZX,
                                   orientation2=Orientation.YZ,
                                   generate_curves=True)
print "CutLinesOnField(...)"
display_only(cutlines, view)
pv.ResetCamera(view)
print "display_only(cutlines, view)"
sleep(DELAY)

xy_view = pv.CreateXYPlotView()
print "pv.CreateXYPlotView()"
index = 0
for curve in curves:
    xyrep = pv.Show(curve, xy_view)
    xyrep.AttributeType = 'Point Data'
    xyrep.UseIndexForXAxis = 0
    xyrep.XArrayName = 'arc_length'
    pv.Render(xy_view)
    set_visible_lines(xyrep, [field_name])
    xyrep.SeriesLabel = [field_name, 'Y' + str(index)]
    r = str(random.random())
    g = str(random.random())
    b = str(random.random())
    xyrep.SeriesColor = [field_name, r, g, b]
    index += 1

pv.Render(xy_view)
sleep(DELAY)

# Create one more view for cut segment
view = pv.CreateRenderView()
reset_view(view)

color = [0.0, 0.7, 0.3]
view.Background = color
pv.Render(view)
sleep(DELAY)

point1 = [0.0, -1.0, 2.5]
point2 = [0.0, 1.0, 2.5]
cutsegment = CutSegmentOnField(med_reader, entity, field_name, timestamp,
                               point1, point2)
print "CutSegmentOnField(...)"
display_only(cutsegment, view)
pv.ResetCamera(view)
print "display_only(cutsegment, view)"
sleep(DELAY)

xy_view = pv.CreateXYPlotView()
curve = pv.Show(cutsegment.Input, xy_view)
curve.AttributeType = 'Point Data'
curve.UseIndexForXAxis = 0
curve.XArrayName = 'arc_length'
set_visible_lines(xyrep, [field_name])

pv.Render(xy_view)
sleep(DELAY)


# Create one more view for animation
view = pv.CreateRenderView()
reset_view(view)

med_file = datadir + "TimeStamps.med"
field_name = "vitesse"
entity = EntityType.NODE
timestamp = 2

med_reader = pv.MEDReader(FileName=med_file)

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
pv.ResetCamera(view)

print "Start Animation"
pv.AnimateReader(med_reader, view)
