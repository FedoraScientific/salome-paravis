#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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
# Author : Anthony Geay

from MEDLoader import *

""" This test checks that umeshes with no names work properly.
"""

fname="testMEDReader4.med"
outImgName="testMEDReader4.png"
#########
coords=DataArrayDouble([(0.,0.,0.),(1.,0.,0.),(2.,0.,0.),(3.,0.,0.),(0.,1.,0.),(1.,1.,0.),(2.,1.,0.),(3.,1.,0.),(0.,2.,0.),(1.,2.,0.),(2.,2.,0.),(3.,2.,0.),(0.,3.,0.),(1.,3.,0.),(2.,3.,0.),(3.,3.,0.)])
m0=MEDCouplingUMesh("mesh",2)
m0.allocateCells()
m0.setCoords(coords)
##
mm=MEDFileUMesh()
mm.setMeshAtLevel(0,m0)
mm.setFamilyFieldArr(1,DataArrayInt([1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]))
#
f1ts0=MEDFileField1TS()
f1ts1=MEDFileField1TS()
#
f0=MEDCouplingFieldDouble(ON_NODES) ; f0.setMesh(m0) ; f0.setName("f0NoPfl")
arr0=DataArrayDouble([0.,1.,2.,3.,1.,1.5,2.2,3.1,2.,2.2,3.,3.1,3.,3.1,3.5,4.])
f0.setArray(arr0)
f0.checkCoherency()
f1ts0.setFieldNoProfileSBT(f0)
#
pfl1=DataArrayInt([0,1,2,3,4,5,6,8,9,12]) ; pfl1.setName("pfl1")
f1=MEDCouplingFieldDouble(ON_NODES) ; f1.setName("f1Pfl")
arr1=DataArrayDouble([3.,2.,1.,0.,2.,1.5,0.,1.,0.,0.2])
f1.setArray(arr1)
f1ts1.setFieldProfile(f1,mm,0,pfl1)
#
fs=MEDFileFields()
fmts0=MEDFileFieldMultiTS()
fmts0.pushBackTimeStep(f1ts0)
fmts1=MEDFileFieldMultiTS()
fmts1.pushBackTimeStep(f1ts1)
fs.pushField(fmts0) ; fs.pushField(fmts1)
mm.write(fname,2)
fs.write(fname,0)
################### MED write is done -> Go to MEDReader

from paraview.simple import *

testMEDReader4_med=MEDReader(FileName=fname)

testMEDReader4_med.AllArrays=['TS0/mesh/ComSup0/f0NoPfl@@][@@P1','TS0/mesh/ComSup0/mesh@@][@@P1']
#testMEDReader4_med.AllTimeSteps=['0000']

Glyph1=Glyph(GlyphType="Sphere",GlyphTransform="Transform2" )
Glyph1.Scalars=['POINTS','f0NoPfl']
Glyph1.SetScaleFactor=0.3
Glyph1.Vectors=['POINTS', '']
Glyph1.GlyphTransform="Transform2"

DataRepresentation1 = Show()
DataRepresentation1.EdgeColor = [0.0, 0.0, 0.5000076295109483]
DataRepresentation1.SelectionPointFieldDataArrayName = 'f0NoPfl'
DataRepresentation1.ScaleFactor = 0.3
DataRepresentation1.ColorArrayName = 'f0NoPfl'
lookupTable = GetLookupTableForArray( "f0NoPfl", 1, RGBPoints=[0.0, 0.23, 0.299, 0.754, 4.0, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0, AllowDuplicateScalars=1 )
function=CreatePiecewiseFunction(Points=[0.0,0.0,0.5,0.0,1.0,1.0,0.5,0.0] )
lookupTable.ScalarOpacityFunction = function
DataRepresentation1.LookupTable = lookupTable

RenderView1 = GetRenderView()
RenderView1.CameraViewUp = [0.06254683966704512, 0.9980420295997885, 0.0]
RenderView1.CameraPosition = [0.8087292125440382, 1.3325993334207897, 12.692130429902462]
RenderView1.CameraClippingRange = [6.326709113742113, 7.168262401281963]
RenderView1.CameraFocalPoint = [0.8087292125440382, 1.3325993334207897, 0.0]
RenderView1.CameraParallelScale = 3.712804729456109

RenderView1.ViewSize =[300,300]
WriteImage(outImgName)
