
if not ('servermanager' in dir()):
  from pvsimple import *

import os

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

    
testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo10(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
