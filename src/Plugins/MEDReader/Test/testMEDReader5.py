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

""" This test is CEA specific one. It generates a .sauv file
that is then read by MEDReader
"""

fname="testMEDReader5.sauv"
outImgName="testMEDReader5.png"
#########
arr=DataArrayDouble(5) ; arr.iota()
c=MEDCouplingCMesh("mesh")
c.setCoords(arr,arr,arr)
m=c.buildUnstructured()
mbis=m.deepCpy() ; mbis.translate([5,0,0]) ; mbis.tetrahedrize(PLANAR_FACE_5)
m=MEDCouplingUMesh.MergeUMeshes([mbis,m]) ; m.setName(c.getName())
mm=MEDFileUMesh()
mm.setMeshAtLevel(0,m)
grp0=DataArrayInt.Range(0,m.getNumberOfCells(),2) ; grp0.setName("grp0")
grp1=DataArrayInt.Range(1,m.getNumberOfCells(),2) ; grp1.setName("grp1")
grp2=DataArrayInt.Range(0,m.getNumberOfNodes(),7) ; grp2.setName("grp2")
mm.setGroupsAtLevel(0,[grp0,grp1])
mm.setGroupsAtLevel(1,[grp2])
ms=MEDFileMeshes()
ms.pushMesh(mm)
f=MEDCouplingFieldDouble(ON_CELLS) ; f.setName("fCell") ; f.setMesh(m[:10])
arr=DataArrayDouble(f.getNumberOfTuplesExpected()) ; arr.iota() ; f.setArray(arr)
f2=MEDCouplingFieldDouble(ON_NODES) ; f2.setName("fNode") ; f2.setMesh(m)
arr=DataArrayDouble(f2.getNumberOfTuplesExpected()) ; arr.iota() ; f2.setArray(arr)

fs=MEDFileFields()
f1ts=MEDFileField1TS()
#f1ts.setFieldNoProfileSBT(f)
pfl=DataArrayInt(10); pfl.iota() ; pfl.setName("pfl") ; f1ts.setFieldProfile(f,mm,0,pfl)
fmts=MEDFileFieldMultiTS()
fmts.pushBackTimeStep(f1ts)
fs.pushField(fmts)
f1ts=MEDFileField1TS()
f1ts.setFieldNoProfileSBT(f2)
fmts=MEDFileFieldMultiTS()
fmts.pushBackTimeStep(f1ts)
fs.pushField(fmts)

mfd=MEDFileData()
mfd.setMeshes(ms)
mfd.setFields(fs)
#mfd.write(fname,2) not activated because useless
#
sw=SauvWriter();
sw.setMEDFileDS(mfd);
sw.write(fname);
del mm,m,c,f,arr
del f1ts
################### MED write is done -> Go to MEDReader
from paraview.simple import *

myMedReader=MEDReader(FileName=fname)
myMedReader.AllArrays = ['TS0/mesh/ComSup1/fNode@@][@@P1']
myMedReader.AllTimeSteps = ['0000']

Clip1=Clip(ClipType="Plane")
Clip1.Input=myMedReader
Clip1.Scalars = ['POINTS', 'FamilyIdNode']
Clip1.ClipType.Origin = [2.0, 2.0, 2.0]
Clip1.ClipType = "Plane"
Clip1.ClipType.Normal = [0.04207410474474753, 0.9319448861971525, 0.3601506612529047]
Clip1.InsideOut = 1

DataRepresentation2 = Show()
DataRepresentation2.EdgeColor = [0.0, 0.0, 0.5000076295109483]
DataRepresentation2.ScalarOpacityUnitDistance = 1.5768745057161244
DataRepresentation2.ExtractedBlockIndex = 1
DataRepresentation2.ScaleFactor = 0.4

RenderView1=GetRenderView()
RenderView1.CenterOfRotation=[2.,2.,2.]
RenderView1.CameraViewUp=[0.24562884954787187,0.6907950752417243,-0.680050463047831]
RenderView1.CameraPosition=[-2.5085697461776486,11.6185941755061,10.14210560568201]
RenderView1.CameraClippingRange=[5.269050059274764,23.64024625330183]
RenderView1.CameraFocalPoint=[2.,2.,2.]
RenderView1.CameraParallelScale=5.071791174723188

LookupTable=GetLookupTableForArray("fNode",1,RGBPoints=[0.0,0.23,0.299,0.754,55.0,0.706,0.016,0.15],VectorMode='Magnitude',NanColor=[0.25,0.0,0.0],ColorSpace='Diverging',ScalarRangeInitialized=1.0,AllowDuplicateScalars=1)
DataRepresentation2 = Show()
DataRepresentation2.EdgeColor = [0.0, 0.0, 0.5000076295109483]
DataRepresentation2.ScalarOpacityUnitDistance = 1.5768745057161244
DataRepresentation2.ExtractedBlockIndex = 1
DataRepresentation2.ScaleFactor = 0.4
DataRepresentation2.ColorArrayName='fNode'
DataRepresentation2.LookupTable=LookupTable
DataRepresentation2.ColorAttributeType='POINT_DATA'


RenderView1.ViewSize=[300,300]
WriteImage(outImgName)
