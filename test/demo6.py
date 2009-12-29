
if not ('servermanager' in dir()):
  from pvsimple import *

import os

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

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo6(fname=pvdata+"/Data/can.ex2", impth=testdir+"/Pic/")
