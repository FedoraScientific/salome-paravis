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

# This case corresponds to: /visu/dump_study/A5 case

from paravistest import datadir, delete_with_inputs
from presentations import *
import pvserver as paravis
from pvsimple import *

my_paravis = paravis.myParavis

settings = {"Offset": [0.0001, 0.0002, 0], "ScalarMode": ("Component", 2), "Position": [0.1, 0.2], "Size": [0.15, 0.25], "Discretize": 1, "NbColors": 44, "NbLabels": 22, "Title": "My presentation", "UseLogScale": 1, "Orientation": 'Horizontal', "ScaleFactor": 0.25, "NbContours": 4}

# 1. TimeStamps.med import
file_path = datadir + "fra.med"
OpenDataFile(file_path)
med_reader = GetActiveSource()
if med_reader is None :
    raise RuntimeError, "TimeStamps.med wasn't imported..."

# 2. Plot3D creation
med_field = "VITESSE"

plot3d = Plot3DOnField(med_reader, EntityType.NODE, med_field, 1, is_contour=True)

# apply settings
plot3d.Position = settings["Offset"]
plot3d.LookupTable.VectorMode = settings["ScalarMode"][0]
plot3d.LookupTable.VectorComponent =  settings["ScalarMode"][1]
plot3d.LookupTable.Discretize = settings["Discretize"]
plot3d.LookupTable.NumberOfTableValues = settings["NbColors"]
plot3d.LookupTable.UseLogScale = settings["UseLogScale"]

slice_filter = plot3d.Input.Input.Input.Input
normal = get_normal_by_orientation(Orientation.ZX, radians(22), radians(33))
bounds = get_bounds(med_reader)
pos = get_positions(1, normal, bounds, 0.11)
slice_filter.SliceType.Normal = normal
slice_filter.SliceOffsetValues = pos

plot3d.Input.Input.ScaleFactor = settings["ScaleFactor"]

contour_filter = plot3d.Input
rgb_points = plot3d.LookupTable.RGBPoints
scalar_range = [rgb_points[0], rgb_points[4]]
surfaces = get_contours(scalar_range, settings["NbContours"])
contour_filter.Isosurfaces = surfaces

bar = get_bar()
bar.Position = settings["Position"]
bar.Position2 = settings["Size"]
bar.NumberOfLabels = settings["NbLabels"]
bar.Title = settings["Title"]
bar.Orientation = settings["Orientation"]

# 3. Dump Study
path_to_save = os.path.join(os.getenv("HOME"), "Plot3D.py")
SaveTrace( path_to_save )

# 4. Delete the created objects, recreate the view
delete_with_inputs(plot3d)
Delete(GetActiveView())
view = CreateRenderView()

# 5. Execution of the created script
execfile(path_to_save)

# 6. Checking of the settings done before dump
recreated_bar = view.Representations[0]
recreated_plot3d = view.Representations[1]

errors = 0
tolerance = 1e-05

# Offset
offset = recreated_plot3d.Position
for i in range(len(settings["Offset"])):
    if abs(offset[i] - settings["Offset"][i]) > tolerance:
        print "ERROR!!! Offset value with ", i, " index is incorrect: ", offset[i], " instead of ", settings["Offset"][i]
        errors += 1

# Scalar mode
vector_mode = recreated_plot3d.LookupTable.VectorMode
vector_component = recreated_plot3d.LookupTable.VectorComponent

if vector_mode != settings["ScalarMode"][0]:
    print "ERROR!!! Vector mode value is incorrect: ",  vector_mode, " instead of ", settings["ScalarMode"][0]
    errors += 1
if vector_component != settings["ScalarMode"][1]:
    print "ERROR!!! Vector component value is incorrect: ",  vector_component, " instead of ", settings["ScalarMode"][1]
    errors += 1

# Position of scalar bar
pos_x = recreated_bar.Position[0]
pos_y = recreated_bar.Position[1]

if abs(pos_x - settings["Position"][0]) > tolerance:
    print "ERROR!!! X coordinate of position of scalar bar is incorrect: ",  pos_x, " instead of ", settings["Position"][0]
    errors += 1
if abs(pos_y - settings["Position"][1]) > tolerance:
    print "ERROR!!! Y coordinate of position of scalar bar is incorrect: ",  pos_y, " instead of ", settings["Position"][1]
    errors += 1

# Size of scalar bar
width  = recreated_bar.Position2[0]
height = recreated_bar.Position2[1]

if abs(width - settings["Size"][0]) > tolerance:
    print "ERROR!!! Width of scalar bar is incorrect: ",  width, " instead of ", settings["Size"][0]
    errors += 1
if abs(height - settings["Size"][1]) > tolerance:
    print "ERROR!!! Height of scalar bar is incorrect: ",  height, " instead of ", settings["Size"][1]
    errors += 1

# Discretize
discretize = recreated_plot3d.LookupTable.Discretize
if discretize != settings["Discretize"]:
    print "ERROR!!! Discretize property is incorrect: ",  discretize, " instead of ", settings["Discretize"]
    errors += 1

# Number of colors
nb_colors = recreated_plot3d.LookupTable.NumberOfTableValues
if nb_colors != settings["NbColors"]:
    print "ERROR!!! Number of colors of scalar bar is incorrect: ",  nb_colors, " instead of ", settings["NbColors"]
    errors += 1

# Number of labels
nb_labels = recreated_bar.NumberOfLabels
if nb_labels != settings["NbLabels"]:
    print "ERROR!!! Number of labels of scalar bar is incorrect: ",  nb_labels, " instead of ", settings["NbLabels"]
    errors += 1

# Title
title = recreated_bar.Title
if title != settings["Title"]:
    print "ERROR!!! Title of presentation is incorrect: ",  title, " instead of ", settings["Title"]
    errors += 1

# Scaling
use_log_scale = recreated_plot3d.LookupTable.UseLogScale
if use_log_scale != settings["UseLogScale"]:
    print "ERROR!!! Scaling of presentation is incorrect: ",  use_log_scale, " instead of ", settings["UseLogScale"]
    errors += 1

# Bar Orientation
orientation = recreated_bar.Orientation
if orientation != settings["Orientation"]:
    print "ERROR!!! Orientation of scalar bar is incorrect: ",  orientation, " instead of ", settings["Orientation"]
    errors += 1

# Scale Factor
scale_factor = recreated_plot3d.Input.Input.ScaleFactor
if abs(scale_factor - settings["ScaleFactor"]) > tolerance:
    print "ERROR!!! Scale factor of presentation is incorrect: ",  scale_factor, " instead of ", settings["ScaleFactor"]
    errors += 1

# Cutting plane
cur_slice_filter = recreated_plot3d.Input.Input.Input.Input

cur_normal = list(cur_slice_filter.SliceType.Normal)
if cur_normal != normal:
    print "ERROR!!! Normal of the cutting plane is incorrect: ",  cur_normal, " instead of ", normal
    errors += 1

cur_pos = list(cur_slice_filter.SliceOffsetValues)
if cur_pos != pos:
    print "ERROR!!! Position of the cuttting plane is incorrect: ",  cur_pos, " instead of ", pos
    errors += 1

# Contours
cur_surfaces = list(recreated_plot3d.Input.Isosurfaces)
if cur_surfaces != surfaces:
    print "ERROR!!! Contours is incorrect: ",  cur_surfaces, " instead of ", surfaces
    errors += 1

if errors > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
