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

# NavigationMode plugin.
# This is auto-start plugin providing navigation
# mode in 3D viewer in Post-Pro style.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(NavigationMode)

# Find ParaView
FIND_PACKAGE(ParaView REQUIRED)
if(NOT ParaView_FOUND)
    MESSAGE(FATAL_ERROR "Please locate ParaView." )
ENDIF(NOT ParaView_FOUND)
INCLUDE(${PARAVIEW_USE_FILE})

# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})

QT4_WRAP_CPP(MOC_SRCS pqSetModeStarter.h)

ADD_PARAVIEW_AUTO_START(IFACES IFACE_SRCS CLASS_NAME pqSetModeStarter
                        STARTUP onStartup)

ADD_PARAVIEW_PLUGIN(NavigationMode "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    SOURCES pqSetModeStarter.cxx ${MOC_SRCS} ${RCS_SRCS} ${IFACE_SRCS})

# Install
INSTALL(
    TARGETS NavigationMode
    RUNTIME DESTINATION lib/paraview
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION lib/paraview
)