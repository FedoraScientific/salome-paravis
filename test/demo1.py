if not ('servermanager' in dir()):
  from pvsimple import *

import os

def demo1(impth):
    """Simple demo that create the following pipeline
    sphere - shrink - \
                       - append
    cone            - /
    """

    # Create a sphere of radius = 0.5, theta res. = 32
    # This object becomes the active source.
    ss = Sphere(Radius=0.5, ThetaResolution=32, PhiResolution=8)

    # The view and representation
    v = GetRenderView()
    if not v:
       v = CreateRenderView()
    v.CameraPosition = [-3, -3, -3]
    v.CameraViewUp = [0, 0, 1]
    v.StillRender()

    # Apply the shrink filter. The Input property is optional. If Input
    # is not specified, the filter is applied to the active source.
    shr = Shrink(Input=ss)
 
    # Create a cone source. 
    cs = Cone()

    # Append cone and shrink
    app = AppendDatasets()
    app.Input = [shr, cs]

    # Show the output of the append filter. The argument is optional
    # as the app filter is now the active object.
    Show(app)
    WriteImage(filename = (impth + "demo1_1.png"), view=v, Magnification=2) 

    PointData = app.GetDataInformation().GetNumberOfPoints()
    CellData = app.GetDataInformation().GetNumberOfCells()
    print PointData, " Points"
    print CellData, " Cells"

    # Render the default view.
    Render()

    
testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
  demo1(impth =testdir+"/Pic/")
