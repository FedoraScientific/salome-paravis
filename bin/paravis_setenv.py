#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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

import os, re

# -----------------------------------------------------------------------------

def set_env( args ):
    """Initialize environment of PARAVIS module"""
    # set PV_PLUGIN_PATH to PARAVIS plug-ins
    paravis_plugin_dir = os.path.join( os.getenv( "PARAVIS_ROOT_DIR" ), "lib", "paraview" )
    plugin_path = re.split( ":|;", os.getenv( 'PV_PLUGIN_PATH', paravis_plugin_dir ) )
    if paravis_plugin_dir not in plugin_path: plugin_path[0:0] = [paravis_plugin_dir]
    os.environ['PV_PLUGIN_PATH'] = ";".join(plugin_path)
    pass


