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

def demo10(fname, impth):
    """This method demonstrates the Contour filter."""

    reader = ExodusIIReader(FileName=fname)
    v=GetActiveView()
    ##Show(reader, view=v)
    contourFilter = Contour(reader)
    contourFilter.ContourBy = 'Temp'
    contourFilter.Isosurfaces
    ##[]

    contourFilter.Isosurfaces = [400]
    camera = GetActiveCamera()
    camera.Position = [0., 60., -33]
    camera.Elevation(-70)
 
    dp2 = GetDisplayProperties(contourFilter)
    contourFilter.PointData[:]
    ##[Array: Temp, Array: V, Array: Pres, Array: AsH3, Array: GaMe3, Array: CH4, Array: H2]

    temp = contourFilter.PointData[0]
    temp.GetRange()
    ## (399.99999999999994, 400.00000000000006)

    dp2.LookupTable = MakeBlueToRedLT(399.999,400)
    dp2.ColorAttributeType = 'POINT_DATA'
    dp2.ColorArrayName = 'Temp'
    Render()
    WriteImage(filename = (impth + "contourFilter_1.png"), view=v, Magnification=2)

    pres = contourFilter.PointData[2]
    pres.GetRange()
    ##(0.0074376024368978605, 0.02845284189526591)

    dp2.LookupTable = MakeBlueToRedLT(0.007437602, 0.02845284)
    dp2.ColorArrayName = 'Pres'
    Render()
    WriteImage(filename = (impth + "contourFilter_2.png"), view=v, Magnification=2)

    dp1 = GetDisplayProperties(reader)
    dp1.Representation = 'Wireframe'
    dp1.LookupTable = dp2.LookupTable
    dp1.ColorAttributeType = 'POINT_DATA'
    dp1.ColorArrayName = 'Pres'
    Show(reader)
 
    camera.Dolly(0.5)
    Render()
    camera.Elevation(-150)
    Render()
    WriteImage(filename = (impth + "contourFilter_3.png"), view=v, Magnification=2)

    
scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )
pvdata = os.getenv("SAMPLES_SRC_DIR")

if __name__ == "__main__":
    demo10(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
