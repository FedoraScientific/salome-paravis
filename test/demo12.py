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

def demo12(fname, impth):
    """This method demonstrates the Stream Tracer."""

    reader = ExodusIIReader(FileName=fname)
    v=GetActiveView()
    streamTracer = StreamTracer(reader)
    streamTracer.Vectors  = 'V'
    streamTracer.SeedType.NumberOfPoints = 100
    streamTracer.SeedType.Radius = 1.15
    streamTracer.MaximumStreamlineLength = 25.

    Show(streamTracer)
    camera = GetActiveCamera()
    camera.Elevation(-45)
    camera.Dolly(0.4)
    dp1 = GetDisplayProperties(streamTracer)
    streamTracer.PointData[:]
    ##[Array: Temp, Array: V, Array: Pres, Array: AsH3, Array: GaMe3, Array: CH4, Array: H2, Array: IntegrationTime, Array: Vorticity, Array: Rotation,     ##Array: AngularVelocity, Array: Normals]
    
    pres = streamTracer.PointData[2]
    dp1.LookupTable = MakeBlueToRedLT(pres.GetRange()[0],pres.GetRange()[1])
    dp1.ColorAttributeType = 'POINT_DATA'
    dp1.ColorArrayName = 'Pres'
    Render()
    WriteImage(filename = (impth + "streamTracer_1.png"), view=v, Magnification=2)


scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )


if __name__ == "__main__":
    demo12(fname=testdir+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
