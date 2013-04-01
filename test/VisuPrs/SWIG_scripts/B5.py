# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

# This case corresponds to: /visu/SWIG_scripts/B5 case

from time import sleep

from paravistest import datadir, tablesdir
from presentations import *
import paravis
import pvsimple


DELAY = 2

my_paravis = paravis.myParavis

# Step 1
print 'Importing "pointe.med"...............',
file_path = datadir + "pointe.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1


# Step 2
view1 = pvsimple.GetRenderView()
print "view1 = pvsimple.GetRenderView()"

sleep(DELAY)

pvsimple.Delete(view1)
print "pvsimple.Delete(view1)"


# Step 3
view2 = pvsimple.CreateRenderView()
print "view2 = pvsimple.CreateRenderView()"

color = [0.0, 0.3, 1.0]
view2.Background = color
print "view2.Background =", str(color)
pvsimple.Render(view2)

scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
print "ScalarMapOnField(...)"
scalarmap.Visibility = 1
pvsimple.Render(view2)

view2.CameraFocalPoint = [0, 0, 0]
print "view2.CameraFocalPoint = [0, 0, 0]"
view2.CameraParallelScale = 2
print "view2.CameraParallelScale = 2"
pvsimple.Render(view2)

view2.ResetCamera()
print "view2.ResetCamera()"
pvsimple.Render(view2)

sleep(DELAY)


# Step 4
view3 = pvsimple.CreateRenderView()
print "view3 = pvsimple.CreateRenderView()"

color = [0.0, 0.7, 0.0]
view3.Background = color
print "view3.Background = ", str(color)
pvsimple.Render(view3)

cutplanes = CutPlanesOnField(med_reader, entity, field_name, timestamp,
                             orientation=Orientation.XY)
print "CutPlanesOnField(...)"

display_only(cutplanes, view3)
print "display_only(cutplanes, view3)"

point = view3.CameraFocalPoint
point[0] = point[0] + 10
view3.CameraFocalPoint = point
print "view3.CameraFocalPoint = ", str(point)

view3.ResetCamera()
print "view3.ResetCamera()"
pvsimple.Render(view3)
print "pvsimple.Render(view3)"

sleep(DELAY)


# Step 5
view4 = pvsimple.CreateRenderView()
print "view4 = pvsimple.CreateRenderView()"

color = [1.0, 0.7, 0.0]
view4.Background = color
print "view5.Background = ", str(color)
pvsimple.Render(view4)

isosurfaces = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
print "isosurfaces = IsoSurfacesOnField(...)"

display_only(isosurfaces, view4)
print "display_only(isosurfaces, view4)"
view4.ResetCamera()
print "view4.ResetCamera()"

pvsimple.Render(view4)
print "pvsimple.Render(view4)"

sleep(DELAY)


# Step 6
view5 = pvsimple.CreateRenderView()
print "view5 = pvsimple.CreateRenderView()"

color = [0.7, 0.7, 0.7]
view5.Background = color
print "view5.Background =", str(color)
pvsimple.Render(view5)

cutlines = CutLinesOnField(med_reader, entity, field_name, timestamp,
                           orientation1=Orientation.ZX,
                           orientation2=Orientation.YZ)
print "cutlines = CutLinesOnField(...)"

display_only(cutlines, view5)
print "display_only(cutlines, view5)"
pvsimple.ResetCamera(view5)
print "pvsimple.ResetCamera(view5)"

sleep(DELAY)


# Step 7
file_path = tablesdir + "sinus.csv"
sinus_csv = pvsimple.CSVReader(FileName=file_path)
sinus_csv.FieldDelimiterCharacters = ' '

view6 = pvsimple.CreateXYPlotView()
print "view6 = pvsimple.CreateXYPlotView()"

xy_rep = pvsimple.Show(sinus_csv)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'x'
xy_rep.SeriesVisibility = ['x', '0']

sleep(DELAY)
