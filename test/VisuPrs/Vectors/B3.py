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

# This case corresponds to: /visu/Vectors/B3 case

import sys

from paravistest import datadir
from presentations import VectorsOnField, EntityType
import paravis
import pvsimple

my_paravis = paravis.myParavis

#====================Stage1: Importing MED file====================

print "**** Stage1: Importing MED file"

print 'Import "ResOK_0000.med"...............',
medFile = datadir + "ResOK_0000.med"
my_paravis.ImportFile(medFile)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

#====================Stage2: Creation of Vectors====================

print "Creating Vectors..........",
vectors = VectorsOnField(med_reader, node_entity, 'vitesse', 1)

if vectors is None:
    print "FAILED"
else:
    print "OK"

#====================Stage3: Change properties======================
glyph = vectors.Input

print "Default LineWidth : ", vectors.LineWidth
print "Default GlyphType : ", glyph.GlyphType
print "Default subtype   : ", glyph.GlyphType.GlyphType
print "Default GlyphPos  : ", glyph.GlyphType.Center

print "Changing value of line width"
width = -10
vectors.LineWidth = width
print "Negative value -10 : ", vectors.LineWidth
pvsimple.Render()

width = 10
vectors.LineWidth = width
print "Positive value  10 : ", vectors.LineWidth
pvsimple.Render()

width = 0
vectors.LineWidth = width
print "Zero value       0 : ", vectors.LineWidth
pvsimple.Render()

width = 2
vectors.LineWidth = width
print "Positive value   2 : ", vectors.LineWidth
pvsimple.Render()

print "Changing glyph type"
gtype = "Arrow"
glyph.GlyphType = gtype
print "Arrow type     : ", glyph.GlyphType
pvsimple.Render()

gtype = "Cone"
glyph.GlyphType = gtype
glyph.GlyphType.Resolution = 3
glyph.GlyphType.Height = 4
print "Cone type: " + str(glyph.GlyphType) + ", resolution: " + str(glyph.GlyphType.Resolution)
pvsimple.Render()

glyph.GlyphType.Resolution = 6
print "Cone type: " + str(glyph.GlyphType) + ", resolution: " + str(glyph.GlyphType.Resolution)
pvsimple.Render()

gtype = "Line"
glyph.GlyphType = gtype
print "Line Type      : ", glyph.GlyphType
pvsimple.Render()

gtype = "2D Glyph"
glyph.GlyphType = gtype
glyph.GlyphType.GlyphType = "Arrow"
pvsimple.Render()

print "Changing glyph position"
pos = [0.0, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Center position : ", glyph.GlyphType.Center
pvsimple.Render()

pos = [0.5, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Tail position   : ", glyph.GlyphType.Center
pvsimple.Render()

pos = [-0.5, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Head position   : ", glyph.GlyphType.Center
pvsimple.Render()
