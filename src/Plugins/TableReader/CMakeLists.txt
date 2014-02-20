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

# TableReader plugin consists of two parts:
# 1. Table reader - reads tables in Post-Pro specific format
# 2. "Table To 3D" filter - creates 3D representation of a table

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
	 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(TableReader)

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

# Add subdirectories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Reader ${CMAKE_CURRENT_SOURCE_DIR}/TableTo3DFilter)
ADD_SUBDIRECTORY(Reader)
ADD_SUBDIRECTORY(TableTo3DFilter)
ADD_SUBDIRECTORY(ParaViewPlugin)