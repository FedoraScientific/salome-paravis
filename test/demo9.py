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

def demo9(fname, impth):
    """This method demonstrates the Threshold filter."""

    reader = ExodusIIReader(FileName=fname)
    v=GetActiveView()
    Show(reader, view=v)
    dp1 = GetDisplayProperties(reader)
    dp1.Representation = 'Wireframe'
    camera = GetActiveCamera()
    camera.Elevation(-60)
    thresholdFilter = Threshold(reader)
    thresholdFilter.Scalars
    ##['POINTS', 'GlobalNodeId']

    thresholdFilter.Scalars = 'Temp'
    Show(thresholdFilter)
    thresholdFilter.ThresholdRange
    ##[0.0, 0.0]
    thresholdFilter.ThresholdRange = [398.55,913.15]
    Render()
    WriteImage(filename = (impth + "thresholdFilter_1.png"), view=v, Magnification=2)

    dp2 = GetDisplayProperties(thresholdFilter)
    thresholdFilter.PointData[:]
    ##[Array: GlobalNodeId, Array: PedigreeNodeId, Array: Temp, Array: V, Array: Pres, Array: AsH3, Array: GaMe3, Array: CH4, Array: H2]

    temp = thresholdFilter.PointData[2]
    temp.GetRange()
    ##(400.46258544921875, 846.694091796875)

    dp2.LookupTable = MakeBlueToRedLT(400.4625,846.6909)
    dp2.ColorAttributeType = 'POINT_DATA'
    dp2.ColorArrayName = 'Temp'
    Render()
    WriteImage(filename = (impth + "thresholdFilter_2.png"), view=v, Magnification=2)
    
    #Set the lower threshold to 500
    thresholdFilter.ThresholdRange[0] = 500
    Render()
    WriteImage(filename = (impth + "thresholdFilter_3.png"), view=v, Magnification=2)
    
    dp1.LookupTable = MakeBlueToRedLT(400.462, 84609)
    dp1.ColorAttributeType = 'POINT_DATA'
    dp1.ColorArrayName = 'Temp'
    Render()
    WriteImage(filename = (impth + "thresholdFilter_4.png"), view=v, Magnification=2)



scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )
pvdata = os.getenv("SAMPLES_SRC_DIR")

if __name__ == "__main__":
    demo9(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
