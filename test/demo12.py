
if not ('servermanager' in dir()):
  from pvsimple import *

import os

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

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo12(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")
