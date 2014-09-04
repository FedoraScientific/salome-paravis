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


if not ('servermanager' in dir()):
  from pvsimple import *

import os, inspect

def demo11(fname, impth):
    """This method demonstrates the Contour filter."""

    reader = ExodusIIReader(FileName=fname)
    v=GetActiveView()
    clipFilter = Clip(reader)
    clipFilter.ClipType = 'Scalar'
 
    clipFilter.Scalars = 'Temp'
    clipFilter.Value = 400
    Show(clipFilter)
    camera = GetActiveCamera()
    camera.Elevation(-75)
    Render()
    WriteImage(filename = (impth + "clipFilter_1.png"), view=v, Magnification=2)

    clipFilter2 = Clip(clipFilter)
    Show(clipFilter2)
    Hide(clipFilter)
    camera.SetPosition(-39, 0, 1.46)
    camera.Roll(90)
    WriteImage(filename = (impth + "clipFilter_2.png"), view=v, Magnification=2)

    dp1 = GetDisplayProperties(reader)
    dp2 = GetDisplayProperties(clipFilter2)
    clipFilter2.PointData[:]
    ##[Array: Temp, Array: V, Array: Pres, Array: AsH3, Array: GaMe3, Array: CH4, Array: H2]

    temp = clipFilter2.PointData[0]
    temp.GetRange()
    ##(399.99999999999994, 913.1500244140625)

    dp2.LookupTable = MakeBlueToRedLT(400,913.15)
    dp2.ColorAttributeType = 'POINT_DATA'
    dp2.ColorArrayName = 'Temp'
    Hide(reader)
    Render()
    WriteImage(filename = (impth + "clipFilter_3.png"), view=v, Magnification=2)

    dp1.Representation = 'Volume'
    temp.GetRange()
    ##(399.99999999999994, 913.1500244140625)
    dp1.LookupTable = dp2.LookupTable
    dp1.ColorAttributeType = 'POINT_DATA'
    dp1.ColorArrayName = 'Temp'
    Show(reader)
    Render()
    WriteImage(filename = (impth + "clipFilter_4.png"), view=v, Magnification=2)


scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )


if __name__ == "__main__":
    demo11(fname=testdir+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
