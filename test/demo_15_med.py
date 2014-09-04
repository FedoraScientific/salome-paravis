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


# ATTENTION. This script is prepared for the future not for current moment !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

if not ('servermanager' in dir()):
  from pvsimple import *

import os, inspect

def LoadMultipleFiles_med(FilePath, FilePrefix):

  TETRA_dom_fl = MEDReader( FileName = [FilePath + FilePrefix + '1' + '.med',
                                                                               FilePath + FilePrefix + '2' + '.med', 
                                                                               FilePath + FilePrefix + '3' + '.med',
                                                                               FilePath + FilePrefix + '4' + '.med',
                                                                               FilePath + FilePrefix + '5' + '.med',
                                                                               FilePath + FilePrefix + '6' + '.med'])
  """
  TETRA_dom_fl.CellArrays = ['Pression', 'Temp.C', 'VitesseX']
  TETRA_dom_fl.GlobalFamilyStatus = ['1 1 1', '1 2 1', '1 3 1', '1 4 1', '1 5 1']
  TETRA_dom_fl.GlobalEntityStatus = ['0 203', '0 304']

  rv = GetRenderView()
  a1_Temp_C_PVLookupTable = CreateLookupTable( RGBPoints=[-1.3, 0.23, 0.3, 0.75, 200., 0.70, 0.016, 0.15],
                                                                                            VectorMode='Magnitude',
                                                                                            ColorSpace='Diverging',
                                                                                            ScalarRangeInitialized=1.0 )

  dr = Show()
  dr.EdgeColor = [0.0, 0.0, 0.50]
  dr.SelectionCellLabelColor = [0.0, 1.0, 0.0]
  dr.SelectionPointLabelJustification = 'Center'
  dr.SelectionCellLabelJustification = 'Center'
  dr.ScalarOpacityFunction = []
  dr.ColorArrayName = 'Temp.C'
  dr.SelectionLineWidth = 2.0
  dr.ScalarOpacityUnitDistance = 2.6738860338205099
  dr.SelectionCellLabelFontSize = 24
  dr.SelectionColor = [1.0, 0.0, 1.0]
  dr.SelectionRepresentation = 'Wireframe'
  dr.LookupTable = a1_Temp_C_PVLookupTable

  rv.CameraPosition = [50.0, -29.75, 267.55447959748415]
  rv.CameraFocalPoint = [50.0, -29.75, 0.0]
  rv.CameraClippingRange = [248.95893480150932, 291.60779679144639]
  rv.CenterOfRotation = [50.0, -29.75, 0.0]
  rv.CameraParallelScale = 69.24819492232271

  Render()
  """


scriptdir = inspect.getframeinfo(inspect.currentframe())[0]
testdir = os.path.dirname( os.path.abspath(scriptdir) )


if __name__ == "__main__":
  LoadMultipleFiles_med(FilePath=testdir+"/TETRA_domaine_fluide/", FilePrefix="TETRA_domaine_fluide")
