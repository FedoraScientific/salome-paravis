
if not ('servermanager' in dir()):
  from pvsimple import *

import os

def demo2(fname, impth):
    """This demo shows the use of readers, data information and display properties."""
   
 # The view
    v = GetRenderView()
    if not v:
       v = CreateRenderView()
    v.CameraPosition = [-20, -9, -45]
    v.CameraFocalPoint = [0.7, 0.3, 1.7]
    v.CameraViewUp = [-0.75, -0.6, 0.25]
    v.CameraViewAngle = [30]
    v.StillRender()

    # Create the exodus reader and specify a file name
    reader = ExodusIIReader(FileName=fname)

    # Get the list of point arrays.
    avail = reader.PointVariables.Available
    print avail

    # Select all arrays
    reader.PointVariables = avail

    # Turn on the visibility of the reader
    Show(reader)

    # Set representation to wireframe
    SetDisplayProperties(Representation = "Wireframe")
   
    # Black background
    SetViewProperties(Background = [0., 0., 0.])
    Render()

    # Change the elevation of the camera.
    GetActiveCamera().Elevation(45)
    Render()

    # Now that the reader executed, let's get some information about it's output.
    pdi = reader[0].PointData

    # This prints a list of all read point data arrays as well as their value ranges.
    print 'Number of point arrays:', len(pdi)
    for i in range(len(pdi)):
        ai = pdi[i]
        print "----------------"
        print "Array:", i, " ", ai.Name, ":"
        numComps = ai.GetNumberOfComponents()
        print "Number of components:", numComps
        for j in range(numComps):
            print "Range:", ai.GetRange(j)

    # White is boring. Let's color the geometry using a variable. First create a lookup table. This object controls how scalar values are mapped to colors. 
    SetDisplayProperties(LookupTable = MakeBlueToRedLT(0.00678, 0.0288))

    # Color by point array called Pres
    SetDisplayProperties(ColorAttributeType = "POINT_DATA")
    SetDisplayProperties(ColorArrayName = "Pres")

    WriteImage(filename = (impth + "demo2_1.png"), view=v, Magnification=2)

    # Set representation to surface
    SetDisplayProperties(Representation = "Surface")

    # Scalar Bar
    lt = MakeBlueToRedLT(0.00678, 0.0288)
    ScalarBar = CreateScalarBar(LookupTable = lt, Title = "Sample")
    GetRenderView().Representations.append(ScalarBar)
    WriteImage(filename = (impth + "demo2_2.png"), view=v, Magnification=2)
    Render()


testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
 demo2(fname=pvdata+"/Data/disk_out_ref.ex2", impth=testdir+"/Pic/")

