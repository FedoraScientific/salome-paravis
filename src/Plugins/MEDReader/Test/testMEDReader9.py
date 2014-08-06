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

""" This test is a non regression test. Test that ExtractGroup adapt to the current mesh of MEDReader. A MED file containing 2 fields on a 2 different meshes.
This test checks that the MeshName follows.
"""

fname="testMEDReader9.med"
#########
m1=MEDCouplingUMesh("m1",2) ; m1.setCoords(DataArrayDouble([(0.,0.),(1.,0.),(0.,1.)])) ; m1.allocateCells() ; m1.insertNextCell(NORM_TRI3,[0,1,2])
m2=MEDCouplingUMesh("m2",2) ; m2.setCoords(DataArrayDouble([(0.,0.),(1.,0.),(0.,1.),(1.,1.)])) ; m2.allocateCells() ; m2.insertNextCell(NORM_QUAD4,[0,1,3,2])
mm1=MEDFileUMesh() ; mm2=MEDFileUMesh()
mm1.setMeshAtLevel(0,m1) ; mm2.setMeshAtLevel(0,m2)
ms=MEDFileMeshes() ; ms.pushMesh(mm1) ; ms.pushMesh(mm2)
f1=MEDCouplingFieldDouble(ON_CELLS) ; f1.setName("f1") ; f1.setMesh(m1) ; f1.setArray(DataArrayDouble([1.111]))
f2=MEDCouplingFieldDouble(ON_CELLS) ; f2.setName("f2") ; f2.setMesh(m2) ; f2.setArray(DataArrayDouble([2.2222]))
ff1=MEDFileField1TS() ; ff2=MEDFileField1TS()
ff1.setFieldNoProfileSBT(f1) ; ff2.setFieldNoProfileSBT(f2)
ms.write(fname,2)
ff1.write(fname,0)
ff2.write(fname,0)
################### MED write is done -> Go to MEDReader
from paraview.simple import *

myMedReader=MEDReader(FileName=fname)
myMedReader.AllArrays=['TS0/m1/ComSup0/f1@@][@@P0']
myMedReader.UpdatePipelineInformation()
ExtractGroup1=ExtractGroup(Input=myMedReader)
ExtractGroup1.UpdatePipelineInformation()
assert(ExtractGroup1.GetProperty("MeshName")[0]=="m1")
myMedReader.AllArrays=['TS0/m2/ComSup0/f2@@][@@P0']
myMedReader.UpdatePipeline()
ExtractGroup2=ExtractGroup(Input=myMedReader)
ExtractGroup2.UpdatePipelineInformation()
assert(ExtractGroup2.GetProperty("MeshName")[0]=="m2")
