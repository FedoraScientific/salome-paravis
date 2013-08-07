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

# This case corresponds to: /visu/3D_viewer/B2 case
# Create 3D Viewer and test set view properties for CutPlanes presentation
# Author: POLYANSKIKH VERA
import sys
import time

from paravistest import *
from presentations import *
from pvsimple import *
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir("3D_viewer/B2")

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
myParavis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

# Split
a_view_r = CreateRenderView()
Delete(a_view_r)

# Split
a_view_l = CreateRenderView()
Delete(a_view_l)

# Split
a_view_t = CreateRenderView()
Delete(a_view_t)

# Split
a_view_b = CreateRenderView()
reset_view(a_view_b)
Render(a_view_b)
