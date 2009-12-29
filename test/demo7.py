
if not ('servermanager' in dir()):
  from pvsimple import *

import os

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

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo7(fname=pvdata+"/Data/can.ex2", impth=testdir+"/Pic/")
