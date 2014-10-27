# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
# Author : Adrien Bruneton (CEA)
#

import PVSERVER_ORB__POA
import SALOME_ComponentPy
import SALOME_DriverPy
import SALOMEDS
import SALOME
import PVSERVER_utils
import subprocess as subp
import socket
from time import sleep
import os
#from SALOME_utilities import MESSAGE

def MESSAGE(m):
    pass
    #os.system("echo \"%s\" >> /tmp/paravis_log.txt" % m)

class PVSERVER_Impl:
    """ The core implementation (non CORBA, or Study related).
        See the IDL for the documentation.
    """
    MAX_PVSERVER_PORT_TRIES = 10
    PVSERVER_DEFAULT_PORT = 11111
    
    def __init__(self):
        self.pvserverPort = -1
        self.pvserverPop = None  # Popen object from subprocess module
        self.lastTrace = ""
        self.isGUIConnected = False  # whether there is an active connection from the GUI.
        try:
            import paraview
            tmp=paraview.__file__
        except:
            raise Exception("PVSERVER_Impl.__init__ : \"import paraview\" failed !")
        # deduce dynamically PARAVIEW_ROOT_DIR from the paraview module location
        self.PARAVIEW_ROOT_DIR = None
        ZE_KEY_TO_FIND_PV_ROOT_DIR="lib"
        li=tmp.split(os.path.sep) ; li.reverse()
        if ZE_KEY_TO_FIND_PV_ROOT_DIR not in li:
            raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                      "PVSERVER_Impl.__init__ : error during dynamic deduction of PARAVIEW_ROOT_DIR : Loc of paraview module is \"%s\" ! \"%s\" is supposed to be the key to deduce it !"%(tmp,ZE_KEY_TO_FIND_PV_ROOT_DIR),
                      "PVSERVER.py", 0))
        li=li[li.index("lib")+1:] ; li.reverse()
        self.PARAVIEW_ROOT_DIR = os.path.sep.join(li)

    """
    Private. Identify a free port to launch the PVServer. 
    """
    def __getFreePort(self, startPort):
        cnt = 0
        currPort = startPort
        while cnt < self.MAX_PVSERVER_PORT_TRIES:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.bind(('', currPort))
                s.close()
                return currPort
            except socket.error as e:
                cnt += 1
                currPort += 1
                pass
        raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                            "[PVSERVER] maximum number of tries to retrieve a free port for the PVServer",
                            "PVSERVER.py", 0))
                                           
    def FindOrStartPVServer( self, port ):
        MESSAGE("[PVSERVER] FindOrStartPVServer ...")
        host = "localhost"
        alive = True
        if self.pvserverPop is None:
            alive = False
        else:
            # Poll active server to check if still alive
            self.pvserverPop.poll()
            if not self.pvserverPop.returncode is None:  # server terminated
                alive = False
        
        if alive:
            return "cs://%s:%d" % (host, self.pvserverPort)  
          
        # (else) Server not alive, start it:
        pvServerPath = os.path.join(self.PARAVIEW_ROOT_DIR, 'bin', 'pvserver')
        opt = []
        if port <= 0:
            port = self.__getFreePort(self.PVSERVER_DEFAULT_PORT)
        self.pvserverPop = subp.Popen([pvServerPath, "--multi-clients", "--server-port=%d" % port])
        sleep(3)  # Give some time to the server to start up to avoid 
                  # ugly messages on the client side saying that it cannot connect
        # Is PID still alive? If yes, consider that the launch was successful
        self.pvserverPop.poll()
        if self.pvserverPop.returncode is None:
            success = True
            self.pvserverPort = port
            MESSAGE("[PVSERVER] pvserver successfully launched on port %d" % port)
        else:
            raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.INTERNAL_ERROR,
                            "[PVSERVER] Unable to start PVServer on port %d!" % port,
                            "PVSERVER.py", 0))
        return "cs://%s:%d" % (host, self.pvserverPort)

    def StopPVServer( self ):
        MESSAGE("[PVSERVER] Trying to stop PVServer (sending KILL) ...")
        if not self.pvserverPop is None:
            self.pvserverPop.poll()
            if self.pvserverPop.returncode is None:
                # Terminate if still running:
                self.pvserverPop.terminate()
                MESSAGE("[PVSERVER] KILL signal sent.")
                return True
        MESSAGE("[PVSERVER] Nothing to kill.")
        return False
    
    def PutPythonTraceStringToEngine( self, t ):
        self.lastTrace = t
        
    def GetPythonTraceString(self):
        return self.lastTrace
      
    def SetGUIConnected( self, isConnected ):
        self.isGUIConnected = isConnected
        
    def GetGUIConnected( self ):
        return self.isGUIConnected
    
class PVSERVER(PVSERVER_ORB__POA.PVSERVER_Gen,
              SALOME_ComponentPy.SALOME_ComponentPy_i,
              SALOME_DriverPy.SALOME_DriverPy_i,
              PVSERVER_Impl):
    """
    Construct an instance of PVSERVER module engine.
    The class PVSERVER implements CORBA interface PVSERVER_Gen (see PVSERVER_Gen.idl).
    It is inherited from the classes SALOME_ComponentPy_i (implementation of
    Engines::EngineComponent CORBA interface - SALOME component) and SALOME_DriverPy_i
    (implementation of SALOMEDS::Driver CORBA interface - SALOME module's engine).
    """
    def __init__ ( self, orb, poa, contID, containerName, instanceName, 
                   interfaceName ):
        SALOME_ComponentPy.SALOME_ComponentPy_i.__init__(self, orb, poa,
                    contID, containerName, instanceName, interfaceName, 0)
        SALOME_DriverPy.SALOME_DriverPy_i.__init__(self, interfaceName)
        PVSERVER_Impl.__init__(self)
        #
        self._naming_service = SALOME_ComponentPy.SALOME_NamingServicePy_i( self._orb )
        #

    """ Override base class destroy to make sure we try to kill the pvserver
        before leaving.
    """
    def destroy(self):    
        self.StopPVServer()
        # Invokes super():
        SALOME_ComponentPy.destroy(self)
      
    """
    Get version information.
    """
    def getVersion( self ):
        import salome_version
        return salome_version.getVersion("PVSERVER", True)

    def GetIOR(self):
        return PVSERVER_utils.getEngineIOR()

    """
    Create object.
    """
    def createObject( self, study, name ):
        MESSAGE("createObject()")
        self._createdNew = True # used for getModifiedData method
        builder = study.NewBuilder()
        father  = findOrCreateComponent( study )
        object  = builder.NewObject( father )
        attr    = builder.FindOrCreateAttribute( object, "AttributeName" )
        attr.SetValue( name )
        attr    = builder.FindOrCreateAttribute( object, "AttributeLocalID" )
        attr.SetValue( PVSERVER_utils.objectID() )
        pass

    """
    Dump module data to the Python script.
    """
    def DumpPython( self, study, isPublished, isMultiFile ):
        MESSAGE("dumpPython()") 
        abuffer = self.GetPythonTraceString().split("\n")
        if isMultiFile:
            abuffer       = [ "  " + s for s in abuffer ]
            abuffer[0:0]  = [ "def RebuildData( theStudy ):" ]
            abuffer      += [ "  pass" ]
        abuffer += [ "\0" ]
        return ("\n".join( abuffer ), 1)
  
    """
    Import file to restore module data
    """
    def importData(self, studyId, dataContainer, options):
      MESSAGE("importData()")
      # get study by Id
      obj = self._naming_service.Resolve("myStudyManager")
      myStudyManager = obj._narrow(SALOMEDS.StudyManager)
      study = myStudyManager.GetStudyByID(studyId)
      # create all objects from the imported stream
      stream = dataContainer.get()
      for objname in stream.split("\n"):
        if len(objname) != 0:
          self.createObject(study, objname)
      self._createdNew = False # to store the modification of the study information later
      return ["objects"] # identifier what is in this file
 
    def getModifiedData(self, studyId):
      MESSAGE("getModifiedData()")
      if self._createdNew:
        # get study by Id
        obj = self._naming_service.Resolve("myStudyManager")
        myStudyManager = obj._narrow(SALOMEDS.StudyManager)
        study = myStudyManager.GetStudyByID(studyId)
        # iterate all objects to get their names and store this information in stream
        stream=""
        father = study.FindComponent( moduleName() )
        if father:
            iter = study.NewChildIterator( father )
            while iter.More():
                name = iter.Value().GetName()
                stream += name + "\n"
                iter.Next()
        # store stream to the temporary file to send it in DataContainer
        dataContainer = SALOME_DataContainerPy_i(stream, "", "objects", False, True)
        aVar = dataContainer._this()
        return [aVar]
      return []
