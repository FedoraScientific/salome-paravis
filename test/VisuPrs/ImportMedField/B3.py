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

# This case corresponds to: /visu/ImportMedField/B3 case
# Import MED file; create presentations for the given fields.

from paravistest import datadir, Import_Med_Field
import paravis

med_file = datadir + "zzzz121b.med"
field_names = ["RESUZERODEPL____________________", "RESUZEROERRE_ELGA_NORE__________", "RESUZEROSIEF_ELGA_DEPL__________", "RESUZEROSIGM_ELNO_DEPL__________"]
prs_list = [ [0,1,5,6,7], [0,1,5,6,7], [0,1,5,6,7], [0,1,5,6,7,9] ]

Import_Med_Field(paravis.myParavis, med_file, field_names, 1, prs_list)
