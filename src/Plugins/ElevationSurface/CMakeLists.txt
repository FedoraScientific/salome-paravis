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

PROJECT(ElevationSurface)
cmake_minimum_required(VERSION 2.6)

cmake_policy(SET CMP0003 NEW)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

SET(SRCS
    vtkElevationSurfaceFilter.cxx 
    )

ADD_PARAVIEW_PLUGIN(ElevationSurface "1.0"
	SERVER_MANAGER_XML ElevationSurface.xml
	GUI_RESOURCE_FILES ElevationSurfaceClient.xml
	SERVER_MANAGER_SOURCES ${SRCS})

install(TARGETS ${PROJECT_NAME} 
   RUNTIME DESTINATION lib/paraview
   LIBRARY DESTINATION lib/paraview
   ARCHIVE DESTINATION lib/paraview
   )

