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

# This case corresponds to: /visu/dump_study/A7 case

from paravistest import datadir, delete_with_inputs
from presentations import *
import paravis
from pvsimple import *

my_paravis = paravis.myParavis

# StreamLines settings
settings = {'name': 'myStreamLines',
            'IntegrationDirection': 'FORWARD',
            'InitialStepLength': 0.009,
            'IntegrationStepUnit': 'Length',
            'IntegratorType': 'Runge-Kutta 4-5',
            'MaximumError': 1.45e-06,
            'MinimumStepLength': 0.0079917,
            'MaximumStepLength': 0.008,
            'MaximumSteps': 475,
            'MaximumStreamlineLength': 1.185,
            'SeedType.Center': [0.1088, 0.03254, 0.431],
            'SeedType.NumberOfPoints': 33,
            'SeedType.Radius': 0.0035}

# errors counter
errors = 0

# 1. TimeStamps.med import
file_path = datadir + "TimeStamps.med"
my_paravis.ImportFile(file_path)
med_reader = GetActiveSource()
if med_reader is None :
    raise RuntimeError, "TimeStamps.med wasn't imported..."

# 2. StreamLines creation
field_name = "vitesse"

source = MergeBlocks(med_reader)
calc = get_add_component_calc(source, EntityType.NODE, field_name)
vector_array = calc.ResultArrayName
calc.UpdatePipeline()
source = calc
stream = pvs.StreamTracer(source)

# 3. Apply settings
RenameSource(settings['name'], stream)
stream.IntegrationDirection = settings['IntegrationDirection']
stream.InitialStepLength = settings['InitialStepLength']
stream.IntegrationStepUnit = settings['IntegrationStepUnit']
stream.IntegratorType = settings['IntegratorType']
stream.MaximumError = settings['MaximumError']
stream.MinimumStepLength = settings['MinimumStepLength']
stream.MaximumStepLength = settings['MaximumStepLength']
stream.MaximumSteps = settings['MaximumSteps']
stream.MaximumStreamlineLength = settings['MaximumStreamlineLength']
stream.SeedType.Center = settings['SeedType.Center']
stream.SeedType.NumberOfPoints = settings['SeedType.NumberOfPoints']
stream.SeedType.Radius = settings['SeedType.Radius']

# 4. Dump Study
path_to_save = os.path.join(os.getenv("HOME"), "StreamLines.py")
SaveTrace( path_to_save )

# 4. Delete the created objects
delete_with_inputs(stream)

# 5. Execution of the created script
execfile(path_to_save)

# 6. Find the recreated StreamTracer object
recreated_stream = FindSource(settings['name'])
if recreated_stream is None:
    raise RuntimeError, "There is no StreamLines in the study (must be created by executed python script)!!!"

print settings['name'] + " was found!!!"

# 7. Check settings

# IntegrationDirection
param = stream.IntegrationDirection
if param != settings['IntegrationDirection']:
    print "ERROR!!! IntegrationDirection of presentation is incorrect: ",  param, " instead of ", settings["IntegrationDirection"]
    errors += 1

# InitialStepLength
param = stream.InitialStepLength
if param != settings['InitialStepLength']:
    print "ERROR!!! InitialStepLength of presentation is incorrect: ",  param, " instead of ", settings["InitialStepLength"]
    errors += 1

# IntegrationStepUnit
param = stream.IntegrationStepUnit
if param != settings['IntegrationStepUnit']:
    print "ERROR!!! IntegrationStepUnit of presentation is incorrect: ",  param, " instead of ", settings["IntegrationStepUnit"]
    errors += 1

# IntegratorType
param = stream.IntegratorType
if param != settings['IntegratorType']:
    print "ERROR!!! IntegratorType of presentation is incorrect: ",  param, " instead of ", settings["IntegratorType"]
    errors += 1

# MaximumError
param = stream.MaximumError
if param != settings['MaximumError']:
    print "ERROR!!! MaximumError of presentation is incorrect: ",  param, " instead of ", settings["MaximumError"]
    errors += 1

# MinimumStepLength
param = stream.MinimumStepLength
if param != settings['MinimumStepLength']:
    print "ERROR!!! MinimumStepLength of presentation is incorrect: ",  param, " instead of ", settings["MinimumStepLength"]
    errors += 1

# MaximumStepLength
param = stream.MaximumStepLength
if param != settings['MaximumStepLength']:
    print "ERROR!!! MaximumStepLength of presentation is incorrect: ",  param, " instead of ", settings["MaximumStepLength"]
    errors += 1

# MaximumSteps
param = stream.MaximumSteps
if param != settings['MaximumSteps']:
    print "ERROR!!! MaximumSteps of presentation is incorrect: ",  param, " instead of ", settings["MaximumSteps"]
    errors += 1

# MaximumStreamlineLength
param = stream.MaximumStreamlineLength
if param != settings['MaximumStreamlineLength']:
    print "ERROR!!! MaximumStreamlineLength of presentation is incorrect: ",  param, " instead of ", settings["MaximumStreamlineLength"]
    errors += 1

# SeedType.Center
param = list(stream.SeedType.Center)
if param != settings['SeedType.Center']:
    print "ERROR!!! SeedType.Center of presentation is incorrect: ",  param, " instead of ", settings["SeedType.Center"]
    errors += 1

# SeedType.NumberOfPoints
param = stream.SeedType.NumberOfPoints
if param != settings['SeedType.NumberOfPoints']:
    print "ERROR!!! SeedType.NumberOfPoints of presentation is incorrect: ",  param, " instead of ", settings["SeedType.NumberOfPoints"]
    errors += 1

# SeedType.Radius
param = stream.SeedType.Radius
if param != settings['SeedType.Radius']:
    print "ERROR!!! SeedType.Radius of presentation is incorrect: ",  param, " instead of ", settings["SeedType.Radius"]
    errors += 1

if errors > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
