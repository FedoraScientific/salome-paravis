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

import  os

def demo3(impth):
    """This method demonstrates an artificial data sources,
    probes it with a line, etc"""

    # The TWO views
    rv = GetRenderView()
    
    v = CreateRenderView()  
    v.CameraPosition = [-30, -20, -65]
    v.CameraFocalPoint = [0.0, 0.0, 0.0]
    v.CameraViewUp = [-0.65, -0.55, 0.5]
    v.CameraViewAngle = [30]
    SetActiveView(rv)

    src = Wavelet()
    # Get some information about the data. First, for the source to execute
    UpdatePipeline()

    di = src.GetDataInformation()
    print "Data type:", di.GetPrettyDataTypeString()
    print "Extent:", di.GetExtent()

    ##src.Representation = 3 # outline
    Show(src, view=v)
    v.StillRender()

    # Apply a contour filter
    cf = Contour(Input = src)
    cf.ContourBy = ['POINTS', 'RTData']
    cf.Isosurfaces = [37.353099999999998, 63.961533333333335, 90.569966666666673, 117.1784, 143.78683333333333, 170.39526666666666, 
                                 197.00370000000001, 223.61213333333333, 250.22056666666668, 276.82900000000001]
    Show(cf, view=rv)

    #Set background
    SetViewProperties(Background = [.50, .20, .30])
    Render()

    # Reset the camera to include the whole thing
    rv.StillRender()
    rv.ResetCamera()
    rv.CameraPosition = [-2.50, 60, -40]
    rv.CameraFocalPoint = [1.25, 1.0, 1.35]
    rv.CameraViewUp = [-0.955, 0.1, 0.25]
    rv.CameraViewAngle = [30]
    rv.StillRender()

    # Now, let's probe the data
    probe = ResampleWithDataset(Input=src)

    # with a line
    line = Line(Resolution=60)

    # that spans the dataset
    bounds = di.GetBounds()

    print "Bounds: ", bounds
    line.Point1 = bounds[0:6:2]
    line.Point2 = bounds[1:6:2]

    probe.Source = line

    # Render with the line
    Show(line, view=rv)
    rv.StillRender()
    WriteImage(filename = (impth + "demo3_1.png"), view=v, Magnification=2)
    WriteImage(filename = (impth + "demo3_2.png"), view=rv, Magnification=2)

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo3(impth=testdir+"/Pic/")
