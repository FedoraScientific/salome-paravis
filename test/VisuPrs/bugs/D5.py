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

from paravistest import tablesdir
from presentations import *
import paravis
import pvsimple

# Import table from file
print 'Import file with tables....',
file_path = tablesdir + "tables_test.xls"
table_reader = pvsimple.TableReader(FileName=file_path)

if table_reader is None:
    print "FAILED"
else:
    table_reader.UpdatePipeline()
    print "OK"

# Display
table_to_3d = pvsimple.TableTo3D(table_reader)
prs = pvsimple.Show()
#prs.ColorArrayName = 'Table'

available_tables = table_reader.GetPropertyValue("AvailableTables")
tables_count = len(available_tables)
for table_nb in range(0, tables_count):
    table_reader.TableNumber = table_nb

    field_name = 'Table'
    vector_mode = 'Magnitude'
    nb_components = 1
    
    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)
    lookup_table.LockScalarRange = 0

    # Set properties
    prs.ColorAttributeType = EntityType.get_pvtype(EntityType.NODE)
    prs.ColorArrayName = field_name
    prs.LookupTable = lookup_table

    # Add scalar bar
    bar = add_scalar_bar(available_tables[table_nb], nb_components, vector_mode,
                         lookup_table, "")
 
    reset_view()
    pvsimple.Delete(bar)
