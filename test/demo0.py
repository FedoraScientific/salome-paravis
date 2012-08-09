# Copyright (C) 2010-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

""" This module can be used to run a simple rendering benchmark test. This
test renders a sphere with various rendering settings and reports the rendering
rate achieved in triangles/sec. """

if not ('servermanager' in dir()):
  from pvsimple import *

import time, sys, os

def render(ss, v, title, nframes):
  print '============================================================'
  print title
  res = []
  res.append(title)
  for phires in (500, 1000):
    ss.PhiResolution = phires
    c = v.GetActiveCamera()
    v.CameraPosition = [-3, 0, 0]
    v.CameraViewUp = [0, 0, 1]
    v.StillRender()
    c1 = time.time()
    for i in range(nframes):
      c.Elevation(0.5)
      v.StillRender()
      sys.stdout.write(".")
      sys.stdout.write("\n")
    tpr = (time.time() - c1)/nframes
    ncells = ss.GetDataInformation().GetNumberOfCells()
    print tpr, " secs/frame"
    print ncells, " polys"
    print ncells/tpr, " polys/sec"
    
    res.append((ncells, ncells/tpr))

  return res

def run(filesour, impth, nframes):

  """ Runs the benchmark. If a filename is specified, it will write the
  results to that file as csv. The number of frames controls how many times
  a particular configuration is rendered. Higher numbers lead to more accurate
  averages. """

  # Create a sphere source to use in the benchmarks
  ss = Sphere(Radius=0.5, ThetaResolution=32, PhiResolution=8)

  # The view and representation
  v = GetRenderView()
  if not v:
    v = CreateRenderView()
  rep = None
  rep = GetRepresentation(proxy=None, view=v)
  results = []

  # Start with these defaults
  #v.UseImmediateMode = 0
  #v.UseTriangleStrips = 0
  
  # Test different configurations
  title = 'display lists, no triangle strips, solid color'
  #v.UseImmediateMode = 0
  #v.UseTriangleStrips = 0
  results.append(render(ss, v, title, nframes))
  WriteImage(filename = (impth + "demo0_1.png"), view=v, Magnification=2)

  title = 'display lists, triangle strips, solid color'
  #v.UseTriangleStrips = 1
  results.append(render(ss, v, title, nframes))
  #WriteImage(filename = (impth + "demo0_2.png"), view=v, Magnification=2)

  title = 'no display lists, no triangle strips, solid color'
  #v.UseImmediateMode = 1
  #v.UseTriangleStrips = 0
  results.append(render(ss, v, title, nframes))
  #WriteImage(filename = (impth + "demo0_3.png"), view=v, Magnification=2)

  title = 'no display lists, triangle strips, solid color'
  #v.UseTriangleStrips = 1
  results.append(render(ss, v, title, nframes))
  #WriteImage(filename = (impth + "demo0_4.png"), view=v, Magnification=2)

  # Color by normals
  lt = MakeBlueToRedLT(-1, 1)
  rep.LookupTable = lt
  rep.ColorAttributeType = 0 # point data
  rep.ColorArrayName = "Normals"
  lt.RGBPoints = [-1, 0, 0, 1, 0.0288, 1, 0, 0]
  lt.ColorSpace = 1 # HSV
  lt.VectorComponent = 0
  
  title = 'display lists, no triangle strips, color by array'
  #v.UseImmediateMode = 0
  #v.UseTriangleStrips = 0
  results.append(render(ss, v, title, nframes))
  #WriteImage(filename = (impth + "demo0_5.png"), view=v, Magnification=2)

  title = 'display lists, triangle strips, color by array'
  #v.UseTriangleStrips = 1
  results.append(render(ss, v, title, nframes))
  #v.UseImmediateMode = 1
  #WriteImage(filename = (impth + "demo0_6.png"), view=v, Magnification=2)

  title = 'no display lists, no triangle strips, color by array'
  #v.UseOCImmediateMode = 1
  #v.UseTriangleStrips = 0
  results.append(render(ss, v, title, nframes))
  #WriteImage(filename = (impth + "demo0_7.png"), view=v, Magnification=2)

  title = 'no display lists, triangle strips, color by array'
  #v.UseTriangleStrips = 1
  results.append(render(ss, v, title, nframes))
  WriteImage(filename = (impth + "demo0_8.png"), view=v, Magnification=2)

  newr = []
  for r in v.Representations:
   if r != rep:
     newr.append(r)
   v.Representations = newr
  
  ss = None
  rep = None
  
  v.StillRender()
  v = None

  if filesour:
    f = open(filesour, "w")
  else:
    f = sys.stdout
  print >>f, 'configuration, %d, %d' % (results[0][1][0], results[0][2][0])
  for i in results:
    print >>f, '"%s", %g, %g' % (i[0], i[1][1], i[2][1])  


testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
  run(filesour=testdir + "/Pic/Information.txt", impth=testdir + "/Pic/", nframes=10)
