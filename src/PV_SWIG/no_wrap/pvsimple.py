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

import paraview
import paravis

# Forward namespace of simple into current pvsimple:
from paraview import simple
for name in dir(simple):
  if not name.startswith("__"):
    globals()[name] = getattr(simple, name)
del simple

def __my_log(msg):
    print "[PARAVIS] %s" % msg

def SalomeConnectToPVServer():
    """
    Automatically connect to the right PVServer when not ("inside SALOME GUI" and "already connected").
    """
    __my_log("Connecting to PVServer ...")
    server_url = ""
    try:
        isGUIConnected = paravis.myParavisEngine.GetGUIConnected()
        if isGUIConnected and paraview.fromGUI:
            __my_log("Importing pvsimple from GUI and already connected. Won't reconnect.")
            return
        server_url = paravis.myParavisEngine.FindOrStartPVServer(0)
        # Extract host and port from URL:
        a = server_url.split(':')
        b = a[1].split('//')
        host, port = b[-1], int(a[-1])
        Connect(host, port)
        __my_log("Connected to %s!" % server_url)
    except Exception as e:
        __my_log("*******************************************")
        __my_log("** Could not connect to a running PVServer!")
        __my_log("*******************************************")
        raise e
    pass

SalomeConnectToPVServer()
