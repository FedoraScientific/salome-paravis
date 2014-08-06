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

# This case corresponds to: /visu/animation/E4 case
# Create animation for Plot 3D for 'pression' field of the the given MED file and dumps picture files in JPEG format %

import sys
import os
from paravistest import * 
from presentations import *
from pvsimple import *
import paravis

# import file
myParavis = paravis.myParavis

# for saving snapshots
picturedir = get_picture_dir("Animation/E4")

theFileName = datadir +  "TimeStamps.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

print "Creating a Viewer.........................",
aView = GetRenderView()
reset_view(aView)
Render(aView)

if aView is None : print "Error"
else : print "OK"

# Plot 3D  creation
prs= Plot3DOnField(aProxy,EntityType.CELL,'pression' , 2)
prs.Visibility=1
aView.ResetCamera()
print "Creating an Animation.....................",
my_format = "jpeg"
print "Current format to save snapshots: ",my_format
# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

# Select only the current field:
aProxy.AllArrays = []
aProxy.UpdatePipeline()
aProxy.AllArrays = ['TS0/dom/ComSup0/pression@@][@@P0']
aProxy.UpdatePipeline()
   
# Animation creation and saving into set of files into picturedir
scene = AnimateReader(aProxy,aView,picturedir+"E4_dom."+my_format)
nb_frames = len(scene.TimeKeeper.TimestepValues)

pics = os.listdir(picturedir) 
if len(pics) != nb_frames:
   print "FAILED!!! Number of made pictures is equal to ", len(pics), " instead of ", nb_frames
    
for pic in pics:
    os.remove(picturedir+pic)    
    
# Prepare animation  performance    
scene.PlayMode = 1 #  set RealTime mode for animation performance
# set period
scene.Duration = 30 # correspond to set the speed of animation in VISU 
scene.GoToFirst()
print "Animation.................................",
scene.Play()
scene.GoToFirst()
