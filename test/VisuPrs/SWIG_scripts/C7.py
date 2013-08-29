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

# This case corresponds to: /visu/SWIG_scripts/C7 case
# Import a table in Post-Pro specific format from file;
# create 3D representation of the table and show it in 3D viewer

import sys

from paravistest import tablesdir, get_picture_dir, pictureext
from presentations import *
import paravis
import pvsimple


# Import table from file
print 'Import file with tables....',
file_path = tablesdir + "table_test.xls"
table_reader = pvsimple.TableReader(FileName=file_path)
if table_reader is None:
    print "FAILED"
else:
    print "OK"

# Get default settings of the reader
available_tables = table_reader.GetPropertyValue("AvailableTables")
is_detect_num = table_reader.DetectNumericColumns
first_str_as_title = table_reader.FirstStringAsTitles
delimiter = table_reader.ValueDelimiter
table_nb = table_reader.TableNumber

print "Default reader settings:"
print "Available tables: ", available_tables
print "Detect Numeric Columns: ", is_detect_num
print "Interpret First String As Column Titles: ", first_str_as_title
print "Value Delimiter: '%s'" % delimiter
print "Table Number: ", table_nb


# Create 3D representation of the table with the
# help of "Table To 3D" filter,
# in Post-Pro this representation is known as "Pointmap3d"
table_to_3d = pvsimple.TableTo3D(table_reader)

print 'Create "Table To 3D" filter....',
if table_to_3d is None:
    print "FAILED"
else:
    print "OK"

print "Default 'Table To 3D' filter settings:"
print "Scale Factor: ", table_to_3d.ScaleFactor
print "Use Optimus Scale: ", table_to_3d.UseOptimusScale
print "Presentation Type: ", table_to_3d.PresentationType
print "Number Of Contours: ", table_to_3d.NumberOfContours


# Show table 3D representation

# Get 3D view
print 'Get 3D view...................',
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    reset_view(view)
    print "OK"

pointmap3d = pvsimple.GetRepresentation(table_to_3d)

table_name = available_tables
if type(available_tables) == list:
  table_name = available_tables[table_nb]

vector_mode = 'Magnitude'
nb_components = 1
entity = EntityType.NODE

# Get lookup table
lookup_table = get_lookup_table(table_name, nb_components, vector_mode)
lookup_table.LockScalarRange = 0

# Set properties
pointmap3d.ColorAttributeType = EntityType.get_pvtype(entity)
pointmap3d.ColorArrayName = table_name
pointmap3d.LookupTable = lookup_table

# Add scalar bar
add_scalar_bar(table_name, nb_components, vector_mode,
               lookup_table, "")

# Reset view
reset_view(view=view)

# Write image

# Directory for saving snapshots
picturedir = get_picture_dir("SWIG_scripts/C7")
if not picturedir.endswith(os.sep):
    picturedir += os.sep

file_name = picturedir + "pointmap3d." + pictureext
pvsimple.WriteImage(file_name, view=view, Magnification=1)
