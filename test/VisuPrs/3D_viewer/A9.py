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

# This case corresponds to: /visu/3D_viewer/A9 case
# Create 3D Viewer and test set view properties for Deformed Shape presentation
# Author:       POLYANSKIKH VERA
from paravistest import *
from presentations import *
from pvsimple import *
import sys
import paravis
import time


def check_value(prs, property_name, value, do_raise=1, compare_toler=-1.0):
    # compare just set value and the one got from presentation
    really_set_value = prs.GetPropertyValue(property_name)
    is_equal = 1
    if compare_toler > 0:
        is_equal = (fabs(really_set_value - value) < compare_toler)
    else:
        is_equal = (really_set_value == value)
    if not is_equal:
        msg = str(really_set_value) + " has been set instead"
        if do_raise:
            raise RuntimeError(msg)
        else:
            print (msg)

    return (not is_equal)


def check_all_params(prs, pres_type,  shad, opac, lwid):
    err = check_value(prs, "Representation", pres_type, 0, -1.0)
    err = err + check_value(prs, "Interpolation", shad, 0, -1.0)
    err = err + check_value(prs, "Opacity", opac, 0, compare_prec)
    err = err + check_value(prs, "LineWidth", lwid,  0, compare_prec)
    return err

# Directory for saving snapshots
picturedir = get_picture_dir("3D_viewer/A9")

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
myParavis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

theFileName = datadir + "cube_hexa8_quad4.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
proxy = GetActiveSource()
if proxy is None:
    raise RuntimeError("Error: can't import file.")
else:
    print "OK"

field_name = 'fieldcelldouble'

print "\nCreating scalar_map.......",
scalar_map = ScalarMapOnField(proxy, EntityType.CELL, field_name, 1)
if scalar_map is None:
    raise RuntimeError("Error!!! Presentation wasn't created...")

display_only(scalar_map, my_view)
reset_view(my_view)
Render(my_view)

compare_prec = 0.00001

print "\nChange Presentation Parameters..."
# Group 1 (SHADED)

print "\nCheck in SURFACE:"

call_and_check(scalar_map, "Representation",
RepresentationType.get_name(RepresentationType.SURFACE))
shrink_filter = Shrink(scalar_map.Input)
shrink_filter.ShrinkFactor = 0.8
shrink_filter.UpdatePipeline()
shrinked_scalar_map = GetRepresentation(shrink_filter)
shrinked_scalar_map.ColorAttributeType = EntityType.get_pvtype(EntityType.CELL)
shrinked_scalar_map.ColorArrayName = field_name
shrinked_scalar_map.Representation = scalar_map.Representation
lookup_table = scalar_map.LookupTable
shrinked_scalar_map.LookupTable = lookup_table
scalar_map.Visibility = 0
shrinked_scalar_map.Visibility = 1
Render(my_view)

shape_to_show = shrinked_scalar_map
setShaded(my_view, 1)
call_and_check(shape_to_show, "Interpolation", "Gouraud")
Render(my_view)
call_and_check(shape_to_show, "Opacity", 0.8, 1, compare_prec)
call_and_check(shape_to_show, "LineWidth", 5.0,  1, compare_prec)

time.sleep(1)

# save picture in file
# Construct image file name
pic_name = picturedir + "params_SURFACE_1." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)


call_and_check(shape_to_show, "Representation",
RepresentationType.get_name(RepresentationType.WIREFRAME))
call_and_check(shape_to_show, "Representation",
RepresentationType.get_name(RepresentationType.SURFACE))

time.sleep(1)
picture_name = picturedir + "params_SURFACE_2." + pictureext

# save picture in file
# Construct image file name
pic_name = picturedir + "params_SURFACE_2." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.SURFACE), "Gouraud", 0.8, 5.0)

##call_and_check(scalar_map, "Shrinked", 0, "Shrinked")
##call_and_check(scalar_map, "Shrinked", 1, "Shrinked")

time.sleep(1)

# save picture in file
# Construct image file name
pic_name = picturedir + "params_SURFACE_3." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.SURFACE), "Gouraud", 0.8, 5.0)

call_and_check(shape_to_show, "Interpolation", "Flat")
call_and_check(shape_to_show, "Interpolation", "Gouraud")

time.sleep(1)

# save picture in file
# Construct image file name
pic_name = picturedir + "params_SURFACE_4." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.SURFACE), "Gouraud", 0.8, 5.0)

call_and_check(shape_to_show, "Opacity", 1.0,  1, compare_prec)
call_and_check(shape_to_show, "Opacity", 0.8,  1, compare_prec)

time.sleep(1)

# save picture in file
# Construct image file name
pic_name = picturedir + "params_SURFACE_5." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.SURFACE), "Gouraud", 0.8, 5.0)

# Reset
##call_and_check(shape_to_show, "Shrinked", 0, "Shrinked")
setShaded(my_view, 0)
call_and_check(shape_to_show, "Interpolation", "Flat",)
Render(my_view)
call_and_check(shape_to_show, "Opacity", 1.0, 1, compare_prec)
call_and_check(shape_to_show, "LineWidth", 1.0, 1, compare_prec)

# Group 2 (WIREFRAME)
hide_all(my_view)
print "\nCheck in WIREFRAME:"
scalar_map.Visibility = 1
call_and_check(scalar_map, "Representation",
RepresentationType.get_name(RepresentationType.WIREFRAME), "Representation")
shrink_filter = Shrink(scalar_map.Input)
shrink_filter.ShrinkFactor = 0.8
shrink_filter.UpdatePipeline()
shrinked_scalar_map = GetRepresentation(shrink_filter)
shrinked_scalar_map.ColorAttributeType = EntityType.get_pvtype(EntityType.CELL)
shrinked_scalar_map.ColorArrayName = field_name
shrinked_scalar_map.Representation = scalar_map.Representation
lookup_table = scalar_map.LookupTable
shrinked_scalar_map.LookupTable = lookup_table
scalar_map.Visibility = 0
shrinked_scalar_map.Visibility = 1
shape_to_show = shrinked_scalar_map

setShaded(my_view, 1)
call_and_check(shape_to_show, "Interpolation", "Gouraud")
Render(my_view)
call_and_check(shape_to_show, "Opacity", 0.8, 1, compare_prec)
call_and_check(shape_to_show, "LineWidth", 5.0, 1, compare_prec)

time.sleep(1)

# save picture in file
# Construct image file name
pic_name = picturedir + "params_WIREFRAME_1." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)


call_and_check(shape_to_show, "Representation",
RepresentationType.get_name(RepresentationType.SURFACE))
call_and_check(shape_to_show, "Representation",
RepresentationType.get_name(RepresentationType.WIREFRAME))

time.sleep(1)
# save picture in file
# Construct image file name
pic_name = picturedir + "params_WIREFRAME_2." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.WIREFRAME), "Gouraud", 0.8, 5.0)

##call_and_check(shape_to_show, "Shrinked", 0)
##call_and_check(shape_to_show, "Shrinked", 1)

time.sleep(1)
# save picture in file
# Construct image file name
pic_name = picturedir + "params_WIREFRAME_3." + pictureext
# Show and record the presentation
process_prs_for_test(shrinked_scalar_map, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.WIREFRAME), "Gouraud", 0.8, 5.0)

call_and_check(shape_to_show, "Interpolation", "Flat")
call_and_check(shape_to_show, "Interpolation", "Gouraud")

time.sleep(1)
# save picture in file
# Construct image file name
pic_name = picturedir + "params_WIREFRAME_4." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.WIREFRAME), "Gouraud", 0.8, 5.0)

call_and_check(shape_to_show, "Opacity", 1.0, 1, compare_prec)
call_and_check(shape_to_show, "Opacity", 0.8, 1, compare_prec)

time.sleep(1)
# save picture in file
# Construct image file name
pic_name = picturedir + "params_WIREFRAME_5." + pictureext
# Show and record the presentation
process_prs_for_test(shape_to_show, my_view, pic_name)

err = err + check_all_params(shape_to_show,
RepresentationType.get_name(RepresentationType.WIREFRAME), "Gouraud", 0.8, 5.0)

if err:
    raise RuntimeError("Presentation parameters are not stable")
