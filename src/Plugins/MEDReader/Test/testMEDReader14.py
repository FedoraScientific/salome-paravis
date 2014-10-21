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
# Author : Anthony Geay (EDF R&D)

from MEDLoader import *

""" This test reproduces precisely the bug EDF8655 (PAL 22677) by cheking that multi loc per geo type in a field is correctly managed.
"""

fname="testMEDReader14.med"
outImgName="testMEDReader14.png"

arr0=DataArrayDouble(7) ; arr0.iota()
arr1=DataArrayDouble(5) ; arr1.iota()
c=MEDCouplingCMesh()
c.setCoords(arr0,arr1)
m=c.build1SGTUnstructured()
pfl=DataArrayInt([5,10,11,16,17,23])
m0=m[pfl.buildComplement(24)]
m1=m[pfl] ; m1.simplexize(0)
m=MEDCouplingUMesh.MergeUMeshesOnSameCoords(m1.buildUnstructured(),m0.buildUnstructured())
m.setName("Mesh")
m1=MEDCoupling1SGTUMesh(m.getName(),NORM_SEG2)
m1.setCoords(m.getCoords())
m1.allocateCells()
for i in [[28,21],[21,14],[14,7],[7,0]]:
    m1.insertNextCell(i)
    pass
for i in xrange(6):
    m1.insertNextCell([i,i+1])
    pass
for i in [[6,13],[13,20],[20,27],[27,34]]:
    m1.insertNextCell(i)
    pass
for i in xrange(6,0,-1):
    m1.insertNextCell([28+i,28+i-1])
    pass
#
mm=MEDFileUMesh()
mm.setMeshAtLevel(0,m)
mm.setMeshAtLevel(-1,m1)
mm.write(fname,2)
#
pfl0=DataArrayInt([0,1,2,3]) ; pfl0.setName("PFL000")
pfl1=DataArrayInt([4,5,6,7,8,9]) ; pfl1.setName("PFL001")
pfl2=DataArrayInt([10,11,12,13]) ; pfl2.setName("PFL002")
pfl3=DataArrayInt([13,14,15,16,19,20,23,24,26,27,28,29]) ; pfl3.setName("PFL003")
#
fieldName0="zeField0"
fieldName1="zeField1"
fieldName2="zeField2"
fs0=MEDFileFieldMultiTS()
fs1=MEDFileFieldMultiTS()
fs2=MEDFileFieldMultiTS()
for i in xrange(5):
    f=MEDFileField1TS()
    zePfl0=DataArrayInt.Aggregate(DataArrayInt.Range(0,12,1),pfl3,0) ; zePfl0.setName("PFL")
    fNode=MEDCouplingFieldDouble(ON_GAUSS_PT) ; fNode.setTime(float(i),i,0)
    tmp=m[zePfl0]
    fNode.setName(fieldName0) ; fNode.setMesh(tmp)
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(0,12,1),[0.,0.,1.,0.,0.,1.],[0.1,0.1,0.8,0.1,0.1,0.8],3*[0.16666666666666666])
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(12,24,1),[-1.,-1.,1.,-1.,1.,1.,-1.,1.],[-0.577350,-0.577350,0.577350,-0.577350,0.577350,0.577350,-0.577350,0.577350],4*[1.])
    arr=DataArrayDouble(2*(12*3+12*4)) ; arr.iota(0+1000*i) ; arr.rearrange(2)
    fNode.setArray(arr) ; arr.setInfoOnComponents(["Comp1_2 [m]","Com2_2 [s^2]"]) ; fNode.checkCoherency()
    fNode.setName(fieldName0)
    f.setFieldProfile(fNode,mm,0,zePfl0)
    #
    zePfl1=DataArrayInt.Range(0,14,1) ; zePfl1.setName("PFL")
    fNode=MEDCouplingFieldDouble(ON_GAUSS_PT) ; fNode.setTime(float(i),i,0)
    tmp=m1[zePfl1]
    #
    fNode.setName(fieldName0) ; fNode.setMesh(tmp)
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(0,4,1),[-1.,1.],[-0.33333333333333337,0.33333333333333337],[1.,1.])
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(4,10,1),[-1.,1.],[-0.5,0.,0.5],[1.,1.,1.])
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(10,14,1),[-1.,1.],[-0.6,-0.2,0.2,0.6],[1.,1.,1.,1.])
    arr=DataArrayDouble(2*(4*2+6*3+4*4)) ; arr.iota(100+1000*i) ; arr.rearrange(2)
    fNode.setArray(arr) ; arr.setInfoOnComponents(["Comp1_2 [m]","Com2_2 [s^2]"]) ; fNode.checkCoherency()
    fNode.setName(fieldName0)
    f.setFieldProfile(fNode,mm,-1,zePfl1)
    fs0.pushBackTimeStep(f)
    #
    f=MEDFileField1TS()
    zePfl2=DataArrayInt.Range(10,20,1) ; zePfl2.setName("PFL2")
    fNode=MEDCouplingFieldDouble(ON_GAUSS_PT) ; fNode.setTime(float(i),i,0)
    tmp=m1[zePfl2]
    fNode.setName(fieldName1) ; fNode.setMesh(tmp)
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(0,4,1),[-1.,1.],[-0.6,-0.2,0.2,0.6],[1.,1.,1.,1.])
    fNode.setGaussLocalizationOnCells(DataArrayInt.Range(4,10,1),[-1.,1.],[-0.6666666666666667,-0.33333333333333337,0.,0.33333333333333337,0.6666666666666667],[1.,1.,1.,1.,1.])
    arr=DataArrayDouble(2*(4*4+6*5)) ; arr.iota(500+1000*i) ; arr.rearrange(2)
    fNode.setArray(arr) ; arr.setInfoOnComponents(["C1 [m]","C2 [s^2]"]) ; fNode.checkCoherency()
    f.setFieldProfile(fNode,mm,-1,zePfl2)
    fs1.pushBackTimeStep(f)
    #
    f=MEDFileField1TS()
    zePfl3=DataArrayInt([6,10,11,12,13,17,18,19,20,24,25,26,27,34]) ; zePfl3.setName("PFL3")
    fNode=MEDCouplingFieldDouble(ON_NODES) ; fNode.setTime(float(i),i,0)
    fNode.setName(fieldName2)
    arr=DataArrayDouble(2*14) ; arr.iota(700+1000*i) ; arr.rearrange(2)
    fNode.setArray(arr) ; arr.setInfoOnComponents(["C3 [kg]","C4 [m^2]"])
    f.setFieldProfile(fNode,mm,0,zePfl3)
    fs2.pushBackTimeStep(f)
    pass
fs0.write(fname,0)
fs1.write(fname,0)
fs2.write(fname,0)

#

from paraview.simple import *

paraview.simple._DisableFirstRenderCameraReset()
reader=MEDReader(FileName=fname)
ExpectedEntries=['TS0/Mesh/ComSup0/zeField0_MM0@@][@@GAUSS', 'TS0/Mesh/ComSup1/zeField0_MM1@@][@@GAUSS', 'TS0/Mesh/ComSup2/zeField0_MM2@@][@@GAUSS', 'TS0/Mesh/ComSup2/zeField1_MM0@@][@@GAUSS', 'TS0/Mesh/ComSup3/zeField1_MM1@@][@@GAUSS', 'TS0/Mesh/ComSup4/zeField2@@][@@P1', 'TS1/Mesh/ComSup0/Mesh@@][@@P0']
assert(reader.GetProperty("FieldsTreeInfo")[::2]==ExpectedEntries)
renderView1=GetActiveViewOrCreate('RenderView')
for entry in [[ExpectedEntries[0]],[ExpectedEntries[1]],[ExpectedEntries[2],ExpectedEntries[3]],[ExpectedEntries[4]]]:
    reader=MEDReader(FileName=fname)
    reader.AllArrays=entry
    gaussPoints=GaussPoints(Input=reader)
    gaussPoints.SelectSourceArray="ELGA@0"
    Show(gaussPoints,renderView1)
    pass

#

readerNodeField=MEDReader(FileName=fname)
readerNodeField.AllArrays=[ExpectedEntries[5]]
nodeFieldDisplay=Show(readerNodeField,renderView1)
ColorBy(nodeFieldDisplay,('POINTS','zeField2'))
nodeFieldDisplay.RescaleTransferFunctionToDataRange(True)
zeField2LUT=GetColorTransferFunction('zeField2')
zeField2LUT.RGBPoints=[990.6568528002015, 0.231373, 0.298039, 0.752941, 1009.0416245953584, 0.865003, 0.865003, 0.865003, 1027.4263963905153, 0.705882, 0.0156863, 0.14902]
zeField2LUT.ScalarRangeInitialized=1.
#
renderView1.ResetCamera()
renderView1.InteractionMode = '2D'
renderView1.CameraPosition = [3.0, 2.0, 10000.0]
renderView1.CameraFocalPoint = [3.0, 2.0, 0.0]
renderView1.ViewSize =[300,300]
#
Render()
WriteImage(outImgName)
