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

def demo7(fname, impth):
    """This method demonstrates the Slice filter."""

    v=GetActiveView()
    reader = ExodusIIReader(FileName=fname)
    sliceFilter = Slice(reader)
    Show(sliceFilter)
    Render()
   
    camera = GetActiveCamera()
    camera.Elevation(30)
    camera.SetPosition(17, 40, -25)
    Render()
    WriteImage(filename = (impth + "sliceFilter_1.png"), view=v, Magnification=2)

    sliceFilter.SliceType.Normal

    #The origin of the slice needs to be moved to see slices in the Y-plane
    sliceFilter.SliceType.Origin = [2.5,5,0]
    sliceFilter.SliceType.Normal = [0,1,0]
    Render()
    WriteImage(filename = (impth + "sliceFilter_2.png"), view=v, Magnification=2)

    sliceFilter.SliceType.Normal = [0,0,1]
    Render()
    WriteImage(filename = (impth + "sliceFilter_3.png"), view=v, Magnification=2)

    sliceFilter.SliceType.Normal = [0,1,0]
    sliceFilter.SliceOffsetValues
    sliceFilter.SliceOffsetValues = [-4,-3.11,-2.22,-1.33,-0.44,0.44,1.33,2.22,3.11,4]

    Render()
    WriteImage(filename = (impth + "sliceFilter_4.png"), view=v, Magnification=2)


scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )
pvdata = os.getenv("SAMPLES_SRC_DIR")

if __name__ == "__main__":
    demo7(fname=pvdata+"/Data/can.ex2", impth=testdir+"/Pic/")
