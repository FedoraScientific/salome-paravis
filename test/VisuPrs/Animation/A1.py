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

# This case corresponds to: /visu/animation/A1 case
# Test animation API

import sys
import os
from paravistest import * 
from presentations import *
from pvsimple import *
import paravis

my_paravis = paravis.myParavis

# 1. TimeStamps.med import
print 'Importing "TimeStamps.med"................',
file_path = datadir + "TimeStamps.med"
my_paravis.ImportFile(file_path)
med_reader = GetActiveSource()
if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. CutLines creation
print "Creating Cut Lines........................",
med_field = "vitesse"
cutlines = CutLinesOnField(med_reader, EntityType.NODE, med_field, 1,
			   nb_lines = 20,
			   orientation1=Orientation.XY, orientation2=Orientation.ZX)
if cutlines is None:
    print "FAILED"
else:
    print "OK"

# 3. Display CutLines
print "Getting a Viewer.........................",
view = GetRenderView()
if view is None:
    print "FAILED"
else:
    print "OK"
cutlines.Visibility = 1
Render(view=view)
cutlines.Visibility = 0
Render(view=view)
display_only(cutlines, view=view)
reset_view(view=view)

# 4. Animation
print "Get Animation scene.....................",
scene = GetAnimationScene()
if scene is None:
    print "FAILED"
else:
    print "OK"

print "Duration default... ", scene.Duration
scene.Duration = -10
scene.Duration = 120
scene.Duration = 0
scene.Duration = 30
print "Duration        ... ", scene.Duration
 
print "Loop            ... ", scene.Loop
scene.Loop = 1
print "Loop            ... ", scene.Loop
scene.Loop = 0
print "Loop            ... ", scene.Loop

print "AnimationTime   ... ", scene.AnimationTime

scene.Play()

scene.GoToFirst()
scene.GoToNext()
scene.GoToNext()

print "AnimationTime   ... ", scene.AnimationTime

scene.GoToPrevious()
scene.GoToLast()

scene.Stop()

print "AnimationTime   ... ", scene.AnimationTime

scene.AnimationTime = -1
scene.AnimationTime = scene.TimeKeeper.TimestepValues[1]
scene.AnimationTime = scene.TimeKeeper.TimestepValues[0]

nb_frames = scene.NumberOfFrames
print "NumberOfFrames  ... ", nb_frames

