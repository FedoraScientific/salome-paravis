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
#  File   : paravis.py
#  Module : PARAVIS
#

import os, new

import PARAVIS
import SALOME
import SALOME_Session_idl
import SALOMEDS
import SALOME_ModuleCatalog
from omniORB import CORBA
from time import sleep
from salome import *

myORB = None
myNamingService = None
myLifeCycleCORBA = None
myNamingService = None
myLocalStudyManager = None
myLocalStudy = None
myLocalParavis = None
myDelay = None
mySession = None

## Initialization of paravis server
def Initialize(theORB, theNamingService, theLifeCycleCORBA, theStudyManager, theStudy, theDelay) :
    global myORB, myNamingService, myLifeCycleCORBA, myLocalStudyManager, myLocalStudy
    global mySession, myDelay
    myDelay = theDelay
    myORB = theORB
    myNamingService = theNamingService
    myLifeCycleCORBA = theLifeCycleCORBA
    myLocalStudyManager = theStudyManager
    while mySession == None:
        mySession = myNamingService.Resolve("/Kernel/Session")
    mySession = mySession._narrow(SALOME.Session)
    mySession.GetInterface()
    myDelay = theDelay
    sleep(myDelay)
    myLocalParavis = myLifeCycleCORBA.FindOrLoadComponent("FactoryServer", "PARAVIS")
    myLocalStudy = theStudy
    myLocalParavis.SetCurrentStudy(myLocalStudy)
    myLocalParavis.ActivateModule()
    return myLocalParavis


def ImportFile(theFileName):
    "Import a file of any format supported by ParaView"
    myParavis.ImportFile(theFileName)


def createFunction(theName):
    "Create function - constructor of Paravis object"
    def MyFunction():
        return myParavis.CreateClass(theName)
    return MyFunction


def createConstructors():
    "Create constructor functions according to list of extracted classes"
    g = globals()
    aClassNames = myParavis.GetClassesList();
    for aName in aClassNames:
        g[aName] = createFunction(aName)

## Initialize of a PARAVIS interface  
myParavis = Initialize(orb, naming_service,lcc,myStudyManager,myStudy, 2)

## Initialize constructor functions
createConstructors()

## Initialize Paravis static objects
vtkSMObject = vtkSMObject()
vtkProcessModule = vtkProcessModule()
vtkPVPythonModule = vtkPVPythonModule()
vtkSMProxyManager = vtkSMProxyManager()


