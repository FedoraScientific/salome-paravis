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
# Author: Adrien Bruneton (CEA)

r""" This module is a direct forward to the initial 'simple' module of ParaView.
On top of that it also establishes a connection to a valid PVServer whose address
is provided by the PARAVIS engine.
"""

__DEBUG = 0

def __my_log(msg):
    if __DEBUG:
      print "[PARAVIS] %s" % msg

def __getFromGUI():
    """ Identify if we are running inside SALOME's embedded interpreter.
    @return a value strictly greater than 0 if we are in SALOME's embedded interpreter
    @return 2 if we are in Salome embedded Python console.
    """
    import salome_iapp
    ret = 0
    if salome_iapp.IN_SALOME_GUI:
      ret += 1
    try:
      if __IN_SALOME_GUI_CONSOLE:  # only defined if we are in SALOME's embedded console (not only GUI)
        ret += 1
    except NameError:
      pass
    return ret

def ShowParaviewView():
    """
    If the import is made from SALOME embedded console, the ParaView application needs to 
    be instanciated to avoid a future crash. 
    """
    if __getFromGUI():
      __my_log("Initializing ParaView main elements, please be patient ...")
      import SalomePyQt
      sgPyQt = SalomePyQt.SalomePyQt()
      viewIds = sgPyQt.findViews("ParaView")
      if len(viewIds):
        sgPyQt.setViewVisible(viewIds[0], True)
        sgPyQt.activateView(viewIds[0])
      else:  
        sgPyQt.createView("ParaView")
      # Now let the GUI main loop process the initialization event posted above
      sgPyQt.processEvents()  
      __my_log("ParaView initialized.")

## The below has to called BEFORE importing paraview!!! This is crazy, but it has to be.
ShowParaviewView()  

import paraview
import pvserver
from paraview import simple

def SalomeConnectToPVServer():
    """
    Automatically connect to the right PVServer when not ("inside SALOME GUI" and "already connected").
    """
    __my_log("Connecting to PVServer ...")
    server_url = ""
    try:
        isGUIConnected = paravis.myPVServerService.GetGUIConnected()
        if isGUIConnected and __getFromGUI():
            __my_log("Importing pvsimple from GUI and already connected. Won't reconnect.")
            return
        server_url = paravis.myPVServerService.FindOrStartPVServer(0)
        # Extract host and port from URL:
        a = server_url.split(':')
        b = a[1].split('//')
        host, port = b[-1], int(a[-1])
        simple.Connect(host, port)
        __my_log("Connected to %s!" % server_url)
        if __getFromGUI():
            paravis.myPVServerService.SetGUIConnected(True)
    except Exception as e:
        __my_log("*******************************************")
        __my_log("** Could not connect to a running PVServer!")
        __my_log("*******************************************")
        raise e
    pass
    
if __getFromGUI() < 1:
    # Only if not in GUI (otherwise the createView will do the connection)
    SalomeConnectToPVServer()
del SalomeConnectToPVServer

# Forward namespace of simple into current pvsimple:
for name in dir(simple):
  if not name.startswith("__"):
    globals()[name] = getattr(simple, name)
del simple
   
