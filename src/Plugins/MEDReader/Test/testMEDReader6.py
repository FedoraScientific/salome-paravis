#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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
# Author : Anthony Geay

from MEDLoader import *

""" This test generate a GaussPoint field on a mesh with groups.
The aim of this test is to check that the ExtractGroup works well for GaussPoints
"""

fname="testMEDReader6.med"
outImgName="testMEDReader6.png"
#########
arr=DataArrayDouble([(0,0,0),(1,0,0),(2,0,0),(3,0,0),(0,1,0),(1,1,0),(2,1,0),(3,1,0),(0,2,0),(1,2,0),(2,2,0),(3,2,0),(0,3,0),(1,3,0),(2,3,0),(3,3,0)])
m0=MEDCouplingUMesh("mesh",2) ; m0.setCoords(arr) ; m0.allocateCells()
for elt in [[2,3,6],[3,7,6],[6,9,5],[6,10,9]]:
    m0.insertNextCell(NORM_TRI3,elt)
    pass
for elt in [[0,4,5,1],[5,6,2,1],[4,8,9,5],[6,10,11,7],[8,12,13,9],[9,13,14,10],[10,14,15,11]]:
    m0.insertNextCell(NORM_QUAD4,elt)
    pass
mm=MEDFileUMesh()
mm.setMeshAtLevel(0,m0)
grp0=DataArrayInt([0,1,4,5,7,10]) ; grp0.setName("grp0")
mm.setGroupsAtLevel(0,[grp0])
#
fGauss=MEDCouplingFieldDouble(ON_GAUSS_PT) ; fGauss.setName("fGauss")
fGauss.setMesh(m0)
fGauss.setGaussLocalizationOnType(NORM_TRI3,[0.,0.,1.,0.,0.,1.],[0.1,0.1,0.05,0.9],[0.4,0.6])
fGauss.setGaussLocalizationOnType(NORM_QUAD4,[-1.,-1.,1.,-1.,1.,1.,-1.,1.],[-0.9,-0.9,0.,0.,0.9,0.9],[0.2,0.3,0.5])
fGauss.setArray(DataArrayDouble([0.9,1.3,0.22,1.3,1.62,2.21,1.62,2.72,2.95,2.54,2.25,2.16,1.58,1.05,3.13,2.91,2.82,1.41,1.58,1.95,3.59,3.53,3.59,2.82,2.91,3.13,2.25,2.54,2.95]))
fGauss.getArray().setInfoOnComponent(0,"C0")
fGauss.checkCoherency()
#
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(fGauss)
#
mm.write(fname,2)
f1ts.write(fname,0)
################### MED write is done -> Go to MEDReader
from paraview.simple import *

myMedReader=MEDReader(FileName=fname)
myMedReader.AllArrays = ['TS0/mesh/ComSup0/fGauss@@][@@GAUSS']
myMedReader.AllTimeSteps = ['0000']

ExtractGroup1 = ExtractGroup(Input=myMedReader)
ExtractGroup1.UpdatePipelineInformation()
ExtractGroup1.AllGroups = ['GRP_grp0']

GaussPoints1 = GaussPoints(Input=ExtractGroup1)
GaussPoints1.SelectSourceArray = ['CELLS','ELGA@0']

Glyph1 = Glyph( Input=GaussPoints1, GlyphType="Sphere", GlyphTransform="Transform2" )
Glyph1.GlyphType = "Sphere"
Glyph1.Vectors = ['POINTS', '']
Glyph1.ScaleFactor = 0.29

RenderView1 = GetRenderView()
RenderView1.CameraFocalPoint = [1.5, 1.5, 0.0]
RenderView1.CameraPosition = [1.5, 1.5, 10000.0]
RenderView1.InteractionMode = '3D'
RenderView1.CameraPosition = [1.5, 1.5, 8.196152422706632]
RenderView1.CameraClippingRange = [7.825640906782493, 8.682319698595558]
RenderView1.CameraParallelScale = 2.1213203435596424
RenderView1.CenterOfRotation = [1.5, 1.5, 0.0]

DataRepresentation4 = Show()
DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]
DataRepresentation4.SelectionPointFieldDataArrayName = 'fGauss'
DataRepresentation4.ScaleFactor = 0.3182729169726372

a1_fGauss_PVLookupTable = GetLookupTableForArray( "fGauss", 1, RGBPoints=[0.22, 0.23, 0.299, 0.754, 2.95, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0, AllowDuplicateScalars=1 )
a1_fGauss_PiecewiseFunction = CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )
DataRepresentation4.ColorArrayName = 'fGauss'
DataRepresentation4.LookupTable = a1_fGauss_PVLookupTable
a1_fGauss_PVLookupTable.ScalarOpacityFunction = a1_fGauss_PiecewiseFunction


RenderView1.ViewSize=[300,300]
WriteImage(outImgName)

