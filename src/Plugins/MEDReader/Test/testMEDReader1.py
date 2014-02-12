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

"""
This test focused on ELNO. Here a 2 QUAD4 cells and a single ELNO field 
is defined.
"""
fname="testMEDReader1.med"
outImgName="testMEDReader1.png"

coords=DataArrayDouble([(0,0,0),(2,1,0),(1,0,0),(1,1,0),(2,0,0),(0,1,0)])
m=MEDCouplingUMesh("mesh",2) ; m.setCoords(coords)
m.allocateCells()
m.insertNextCell(NORM_QUAD4,[0,5,3,2])
m.insertNextCell(NORM_QUAD4,[4,2,3,1])
m.finishInsertingCells()
MEDLoader.WriteMesh(fname,m,True)
#
f0=MEDCouplingFieldDouble(ON_GAUSS_NE) ; f0.setMesh(m) ; f0.setTimeUnit("ms")
f0.setTime(1.1,1,1)
f0.setName("myELNOField")
arr=DataArrayDouble([7,5,3,1,5,3,1,7]) ; arr.setInfoOnComponent(0,"Comp0")
f0.setArray(arr)
MEDLoader.WriteFieldUsingAlreadyWrittenMesh(fname,f0)
#
f0.setTime(2.2,2,1)
arr=DataArrayDouble([1,7,5,3,7,5,3,1]) ; arr.setInfoOnComponent(0,"Comp0")
f0.setArray(arr)
MEDLoader.WriteFieldUsingAlreadyWrittenMesh(fname,f0)
#
f0.setTime(3.3,3,1)
arr=DataArrayDouble([3,1,7,5,1,7,5,3]) ; arr.setInfoOnComponent(0,"Comp0")
f0.setArray(arr)
MEDLoader.WriteFieldUsingAlreadyWrittenMesh(fname,f0)
#
f0.setTime(4.4,4,1)
arr=DataArrayDouble([5,3,1,7,3,1,7,5]) ; arr.setInfoOnComponent(0,"Comp0")
f0.setArray(arr)
MEDLoader.WriteFieldUsingAlreadyWrittenMesh(fname,f0)
