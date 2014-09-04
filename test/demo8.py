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

def demo8(fname, impth):
    """This method demonstrates the Glyph filter."""

    reader = ExodusIIReader(FileName=fname)
    v=GetActiveView()
    Show(reader, view=v)
    camera = GetActiveCamera()
    camera.SetPosition(0, 5, -45)
    camera.Elevation(30)

    #Change the can to be wireframe so we can see the glyphs
    dp1 = GetDisplayProperties(reader, view=v)
    dp1.Representation = 'Wireframe'
    glyphFilter = Glyph(reader)

    #We won't see much if we don't move forward in the animation a little bit
    tsteps = reader.TimestepValues
    v = GetActiveView()
    v.ViewTime = tsteps[20]
    glyphFilter.Vectors
    ## ['POINTS', 'DISPL']

    #Set the glyphs to use the acceleration vectors and adjust scale factor
    glyphFilter.Vectors = 'ACCL'
    ##glyphFilter.GetScaleFactor
    ##1.0
    glyphFilter.SetScaleFactor = 0.0000002
    
    Show()
    Render()
    WriteImage(filename = (impth + "glyphFilter_1.png"), view=v, Magnification=2)

    #Set the glyphs to use the velocity vectors and adjust scale factor
    glyphFilter.Vectors = 'VEL'
    glyphFilter.SetScaleFactor = 3e-4
 
    #Add some color
    dp2 = GetDisplayProperties(glyphFilter)
    glyphFilter.PointData[:]
    ##[Array: PedigreeNodeId, Array: DISPL, Array: VEL, Array: ACCL, Array: GlobalNodeId, Array: GlyphVector]

    #We'll color by velocity so use that array.
    vel = glyphFilter.PointData[2]
    vel.GetName()
    ##'VEL'

    vel.GetRange()
    ##(-2479.9521484375, 1312.5040283203125)
    dp2.LookupTable = MakeBlueToRedLT(-2479.9521,1312.5)
    dp2.ColorAttributeType = 'POINT_DATA'
    dp2.ColorArrayName = 'VEL'
    Render()
    WriteImage(filename = (impth + "glyphFilter_2.png"), view=v, Magnification=2)


scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )


if __name__ == "__main__":
    demo8(fname=testdir+"/Data/can.ex2", impth=testdir+"/Pic/")
