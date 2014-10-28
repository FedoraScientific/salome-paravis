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

# This case corresponds to: /visu/bugs2/C5 case

import sys
import os
import time
from paravistest import datadir
from presentations import *
import pvserver as paravis
import pvsimple

my_paravis = paravis.myParavis

medPath = datadir

class DisplayManager:
    """
    Create 3D presentations on entities on a given med file and mesh
    """
    def __init__(self, medFile=None, meshName=None, myEntity=None):
        if ( medFile is not None and meshName is not None and myEntity is not None):
            self.loadData(medFile, meshName, myEntity)
        else:
            self.medFile  = None
            self.meshName = None
            self.myData   = None
            self.myMesh   = None
	    self.myEntity = None
             
    def loadData(self, medFile, meshName=None, myEntity=EntityType.NODE):
        self.medFile  = medFile
        my_paravis.ImportFile(medFile)
        self.myData   = pvsimple.GetActiveSource()
        self.myEntity = myEntity
        if meshName is not None:
            self.setMesh(meshName)

    def getData(self):
        return self.myData
        
    def checkData(self):
        if ( self.myData is None or self.myMesh is None ):
            return False
        return True

    def setMesh(self,meshName):
        self.meshName = meshName
        self.myMesh   = MeshOnEntity(self.myData,
                                     self.meshName,
                                     self.myEntity)
        self.myMesh.Representation = 'Surface'

    def DisplayMap(self, aView, aMap, title, aDelay=0):
        if aMap is None:
            print "Null scalar map is created"        
        display_only(aMap, aView)
        reset_view(aView)
        time.sleep(aDelay)

    def ScalarMap(self, aView, fieldName, iteration, title , delay=0):
        if not self.checkData(): return
        aMap = ScalarMapOnField(self.myData, self.myEntity, fieldName, iteration)
        self.DisplayMap(aView, aMap, title, delay)

    def DeformedShape(self, aView, fieldName, iteration, title , delay=0):
        if not self.checkData(): return
        aMap = DeformedShapeOnField(self.myData, self.myEntity, fieldName, iteration)
        if aMap is not None:
            aMap.ColorArrayName = fieldName
        self.DisplayMap(aView, aMap, title, delay)

    def Vectors(self, aView, fieldName, iteration, title , delay=0):
        if not self.checkData(): return
        aMap = VectorsOnField(self.myData, self.myEntity, fieldName, iteration)
        if aMap is not None:
            aMap.ColorArrayName = fieldName
        self.DisplayMap(aView, aMap, title, delay)

    def IsoSurfaces(self, aView, fieldName, iteration, title , delay=0):
        if not self.checkData(): return
        aMap = IsoSurfacesOnField(self.myData, self.myEntity, fieldName, iteration)
        self.DisplayMap(aView, aMap, title, delay)

    def Animation(self, aView, theObj, theDuration, NbOfLoops, title, aPath=""):

        path = None
        if aPath is not "":
            print "Define save path"
	    path = aPath
            
        scene = pvsimple.AnimateReader(theObj, aView, path)
	
        print "Start Animation"

        scene.Duration = theDuration
        NbOfFrames = len(scene.TimeKeeper.TimestepValues)
        NbOfIter = NbOfFrames-1

        reset_view(view=aView)

        ind =  0
        while ind < NbOfLoops:
            scene.Play()
            ind = ind + 1

        print "Stop Animation"
        scene.Stop()

        return scene

    def XYPlot(self, myView, txtFile, theTableName, theDelay, theColor):
        table = TableReader(FileName=txtFile)

        # >>> Create curve
        myView = CreateXYPlotView()
        myCurve = Show(table, view = myView)
    
def createView():
    aView=pvsimple.GetRenderView()
    return aView


theEntity = EntityType.CELL
theMedFile = "TETRA_domaine_fluide.med"
theMedFile = os.path.join(medPath,theMedFile)
theMeshName = "Domaine_fluide"
theFieldName = "TempC"
theDuration = 20
NbOfLoops = 4
#thePath = os.getenv("TMP_DIR")
thePrefix = "TestPngFromAnim"
thePath = os.path.join("/tmp", thePrefix)
thePath += ".png"

displayMng = DisplayManager()
displayMng.loadData(theMedFile,theMeshName,theEntity)
ScalarMapOnField(displayMng.getData(), theEntity, theFieldName, 1)

myView = createView()
displayMng.Animation(myView, displayMng.getData(), theDuration, NbOfLoops, "", thePath)
