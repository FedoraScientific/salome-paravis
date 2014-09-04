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

def demo6(fname, impth):
    """This method demonstrates the Clip filter."""

    reader = ExodusIIReader(FileName=fname)
    v = CreateRenderView()
    repr = Show(reader, view=v)
    Render()
    v.StillRender()
    v.ResetCamera()
    clipFilter = Clip(reader)
    Hide(reader)
    Show(clipFilter)
    Render()

    #get camera
    camera = GetActiveCamera()
    camera.GetPosition()

    #figure out where you are
    camera.SetPosition(4.156,4.0,-36.29)
    camera.Elevation(30)
    camera.SetPosition(4.15603, 30, -35)
    Render()
    clipFilter.UseValueAsOffset = 1
    clipFilter.Value = 5
    Render()
    WriteImage(filename = (impth + "clipFilter_1.png"), view=v, Magnification=2)

    clipFilter.ClipType.Normal
    clipFilter.ClipType.Origin

    #reset the offset value of the clip to 0
    clipFilter.Value = 0
    clipFilter.ClipType.Normal = [0, 0, 1]
    Render()
    WriteImage(filename = (impth + "clipFilter_2.png"), view=v, Magnification=2)

    clipFilter.Value = -3
    Render()
    WriteImage(filename = (impth + "clipFilter_3.png"), view=v, Magnification=2)

    clipFilter.InsideOut = 1
    Render()
    WriteImage(filename = (impth + "clipFilter_4.png"), view=v, Magnification=2)



scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )
pvdata = os.getenv("SAMPLES_SRC_DIR")

if __name__ == "__main__":
    demo6(fname=pvdata+"/Data/can.ex2", impth=testdir+"/Pic/")
