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

# This case corresponds to: /visu/3D_viewer/B1 case

from paravistest import *
from presentations import *
from pvsimple import *
import paravis

import math

# 1. First viewer creation
view1 = GetRenderView()

# 2. Second viewer creation
view2 = CreateRenderView()

# 3. Change view size
size1 = view1.ViewSize
size2 = view2.ViewSize

w = size1[0] + size2[0]
w1 = math.trunc(w * 0.7)
w2 = w - w1

view1.ViewSize = [w1, size1[1]]
view2.ViewSize = [w2, size2[1]]

# 4. Change view position
h = view1.ViewSize[1]
view1.ViewSize = [h//2, w]
view2.ViewSize = [h//2, w]
view1.ViewPosition = [0, h//2]
view1.ViewPosition = [0, 0]

Render(view1)
Render(view2)
