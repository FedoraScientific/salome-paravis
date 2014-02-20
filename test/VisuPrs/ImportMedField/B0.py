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

# This case corresponds to: /visu/ImportMedField/B0 case
# Import MED file; create presentations for the given fields.

from paravistest import datadir, Import_Med_Field
import paravis

med_file = datadir + "maill.0_volumes.med"
field_names = ["RETH____FLUX_NOEU_TEMP__________", "RETH____TEMP____________________"]
prs_list = [ range(1,10), [1,2,3,4,8] ]

Import_Med_Field(paravis.myParavis, med_file, field_names, 1, prs_list)
