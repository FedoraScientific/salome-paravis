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

# This case corresponds to: /visu/StreamLines/B4 case
# StreamTracer filter properties

from paravistest import datadir
from presentations import *
import paravis
import pvsimple

my_paravis = paravis.myParavis

# 1. Import MED file
print 'Import "ResOK_0000.med"...............',
file_path = datadir + "ResOK_0000.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    print "FAILED"
else:
    print "OK"

# 2. Creating StreamLines
print "Creating Stream Lines.....",
streamlines = StreamLinesOnField(med_reader, EntityType.NODE, 'vitesse', 1)
if streamlines is None:
    print "FAILED"
else:
    print "OK"

# 3. StreamLines parameters
stream_tracer = pvsimple.GetActiveSource()

print "Initial Step Length: ", stream_tracer.InitialStepLength
print "Integration Direction: ", stream_tracer.IntegrationDirection
print "Integration Step Unit: ", stream_tracer.IntegrationStepUnit
print "Integrator Type: ", stream_tracer.IntegratorType
print "Interpolator Type: ", stream_tracer.InterpolatorType
print "Maximum Error: ", stream_tracer.MaximumError
print "Minimum Step Length: ", stream_tracer.MinimumStepLength
print "Maximum Step Length: ", stream_tracer.MaximumStepLength
print "Maximum Steps: ", stream_tracer.MaximumSteps
print "Maximum Streamline Length: ", stream_tracer.MaximumStreamlineLength
print "Seed Type: ", type(stream_tracer.SeedType)
print "Center: ", stream_tracer.SeedType.Center
print "Number Of Points: ", stream_tracer.SeedType.NumberOfPoints
print "Radius: ", stream_tracer.SeedType.Radius
