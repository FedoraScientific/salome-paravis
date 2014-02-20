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

import os

def demo4(fname, animpth):
    """This method demonstrates the AnimateReader for creating animations."""

    reader = ExodusIIReader(FileName=fname)
    v = CreateRenderView()
    repr = Show(reader, view=v)
    v.StillRender()
    v.ResetCamera()
    ##v.CameraPosition = [0., -30, 0]
    ##v.CameraFocalPoint = [6.2, 9.5, -0.5]
    ##v.CameraViewUp = [-0.64, 0.2, -0.75]
    ##v.CameraViewAngle = [30]
    v.StillRender()
    c = v.GetActiveCamera()
    c.Elevation(95)
  
    r = AnimateReader(reader, view=v, filename=(animpth + "can.png"))


testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo4(fname=pvdata+"/Data/can.ex2", animpth=testdir+"/Animation/")
