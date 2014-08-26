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
import salome

## Initialization of paravis server
def Initialize(theDelay) :
    mySession = None
    while mySession == None:
        mySession = salome.naming_service.Resolve("/Kernel/Session")
    mySession = mySession._narrow(SALOME.Session)
    mySession.GetInterface()
    sleep(theDelay)
    myLocalParavis = salome.lcc.FindOrLoadComponent("FactoryServer", "PARAVIS")
    myLocalStudy = salome.myStudy
    myLocalParavis.SetCurrentStudy(myLocalStudy)
    myLocalParavis.ActivateModule()  ## TO BE DISCUSSED!
    return myLocalParavis

def StartOrRetrievePVServerURL():
  """ To be completed!!! Should invoke IDL methods from 'PARAVIS' module"""
  return "localhost"

# def ImportFile(theFileName):
#     "Import a file of any format supported by ParaView"
#     myParavis.ImportFile(theFileName)

## Initialize PARAVIS interface  
myParavisEngine = Initialize(2)

