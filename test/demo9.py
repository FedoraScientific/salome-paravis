
if not ('servermanager' in dir()):
  from pvsimple import *

import os

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

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo9(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
