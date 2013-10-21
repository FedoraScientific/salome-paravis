# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

r""" This module is a direct forward to the initial 
'simple' module of ParaView.
"""

from paraview import simple

for name in dir(simple):
  if name != "__file__":
    globals()[name] = getattr(simple, name)
del simple

print "Connecting to PVServer ..."
try:
  # TODO: this should be improved (retrieval from the engine)
  Connect('localhost')
except Exception as e:
  print "*******************************************"
  print "*******************************************"
  print "Could not connect to PVServer on localhost!"
  print "*******************************************"
  print "*******************************************"
  raise e
