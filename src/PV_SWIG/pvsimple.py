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

r""" This module is a direct forward to the initial 'simple' module of ParaView.
On top of that it also establishes a connection to a valid PVServer whose address
is provided by the PARAVIS engine.
"""

from paraview.simple import *
import paravis        ## Triggers the "FindOrLoadCompo(PARAVIS)"

def SalomeConnectToPVServer():
  print "Connecting to PVServer ..."
  try:
    server_url = paravis.StartOrRetrievePVServerURL()
    Connect(server_url)
  except Exception as e:
    print "*******************************************"
    print "*******************************************"
    print "Could not connect to a running PVServer!"
    print "*******************************************"
    print "*******************************************"
    raise e
  print "Connected!"

# Automatically connect to the right PVServer:
SalomeConnectToPVServer()
