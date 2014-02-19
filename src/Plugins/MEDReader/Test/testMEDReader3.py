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

fname="testMEDReader3.med"
outImgName="testMEDReader3.png"

""" This test checks that invalid double time (not in [-1e299:1e299])
 leads MEDReader to takes iteration numbers instead of double time to discriminate timesteps each other
 See EDF 1956.
"""
#########

m=MEDCouplingUMesh("mesh",2)
coords=DataArrayDouble([(0.,0.,0.),(1.,0.,0.),(1.,1.,0.),(0.,1.,0.)],4,3)
m.setCoords(coords)
m.allocateCells()
m.insertNextCell(NORM_QUAD4,[0,3,2,1])
mm=MEDFileUMesh()
mm.setMeshAtLevel(0,m)
mm.write(fname,2)
fmts=MEDFileFieldMultiTS()
#
f=MEDCouplingFieldDouble(ON_NODES) ; f.setName("ANodeField")
f.setTime(1.7976931348623157e+308,1,1) ; f.setMesh(m)
arr=DataArrayDouble([0.,4.,4.,4.]) ; arr.setInfoOnComponent(0,"aaa")
f.setArray(arr)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f)
fmts.pushBackTimeStep(f1ts)
#
f.setTime(1.7976931348623157e+308,2,2)
arr=DataArrayDouble([1.,4.,4.,4.]) ; arr.setInfoOnComponent(0,"aaa")
f.setArray(arr)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f)
fmts.pushBackTimeStep(f1ts)
#
f.setTime(1.7976931348623157e+308,3,3)
arr=DataArrayDouble([2.,4.,4.,4.]) ; arr.setInfoOnComponent(0,"aaa")
f.setArray(arr)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f)
fmts.pushBackTimeStep(f1ts)
#
f.setTime(1.7976931348623157e+308,4,6)
arr=DataArrayDouble([3.,4.,4.,4.]) ; arr.setInfoOnComponent(0,"aaa")
f.setArray(arr)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f)
fmts.pushBackTimeStep(f1ts)
#
f.setTime(1.7976931348623157e+308,5,7)
arr=DataArrayDouble([4.,4.,4.,4.]) ; arr.setInfoOnComponent(0,"aaa")
f.setArray(arr)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f)
fmts.pushBackTimeStep(f1ts)
#
fmts.write(fname,0)

################### MED write is done -> Go to MEDReader

from paraview.simple import *

AnimationScene1=GetAnimationScene()
AnimationScene1.PlayMode='Snap To TimeSteps'
AnimationScene1.EndTime = 2.0
AnimationScene1.AnimationTime = 1.0
AnimationScene1.StartTime = 1.0

testMEDReader3=MEDReader(FileName=fname)
testMEDReader3.AllArrays=['TS0/mesh/ComSup0/ANodeField@@][@@P1']

assert(list(testMEDReader3.TimestepValues)==[1.,2.,3.,4.,5.]) ## <- the test is here - double time steps are too big use dt.

RenderView1=GetRenderView()
RenderView1.ViewTime=3.0
RenderView1.CameraPosition=[0.5,0.5,5.7320508075688776]
RenderView1.CameraClippingRange=[2.704730299493189,2.7730315696824106]
RenderView1.ViewSize=[300,300]

DataRepresentation2=Show()
DataRepresentation2.EdgeColor=[0.0, 0.0, 0.5000076295109483]
DataRepresentation2.SelectionPointFieldDataArrayName='ANodeField'
DataRepresentation2.SelectionCellFieldDataArrayName='FamilyIdCell'
DataRepresentation2.ScalarOpacityUnitDistance=1.4142135623730951
DataRepresentation2.ExtractedBlockIndex=1
DataRepresentation2.ScaleFactor=0.1

a1_ANodeField_PVLookupTable=GetLookupTableForArray("ANodeField",1,RGBPoints=[0.0,0.23,0.299,0.754,4.0,0.706,0.016,0.15],VectorMode='Magnitude',NanColor=[0.25,0.0,0.0],
                                                     ColorSpace='Diverging',ScalarRangeInitialized=1.0,AllowDuplicateScalars=1)
a1_ANodeField_PiecewiseFunction=CreatePiecewiseFunction(Points=[0.0,0.0,0.5,0.0,1.0,1.0,0.5,0.0])
a1_ANodeField_PVLookupTable.ScalarOpacityFunction=a1_ANodeField_PiecewiseFunction

DataRepresentation2.ScalarOpacityFunction=a1_ANodeField_PiecewiseFunction
DataRepresentation2.ColorArrayName='ANodeField'
DataRepresentation2.LookupTable=a1_ANodeField_PVLookupTable
###
WriteImage(outImgName)


