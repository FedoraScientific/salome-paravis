
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
