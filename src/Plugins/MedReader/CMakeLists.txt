# Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

project(MedReader)
	 
cmake_minimum_required(VERSION 2.8)
	
if(COMMAND cmake_policy)
	 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
		
SET(MED_READER_MAJOR_VERSION 3)
SET(MED_READER_MINOR_VERSION 1)
SET(MED_READER_BUILD_VERSION 0)
SET(MED_READER_VERSION
    "${MED_READER_MAJOR_VERSION}.${MED_READER_MINOR_VERSION}.${MED_READER_BUILD_VERSION}")

#
# Load the ParaView/VTK packages
#
set(MedReader_BUILD_ParaView_Plugin ON CACHE BOOL "Build the ParaView plugin")

if(MedReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(ParaView REQUIRED)
    if(NOT ParaView_FOUND)
        MESSAGE(FATAL_ERROR "Please locate ParaView." )
    ENDIF(NOT ParaView_FOUND)
    INCLUDE(${PARAVIEW_USE_FILE})
else(MedReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(VTK REQUIRED)
    if(NOT VTK_FOUND)
        MESSAGE(FATAL_ERROR "Please locate VTK." )
    ENDIF(NOT VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
endif(MedReader_BUILD_ParaView_Plugin)

	 
# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})
#
# Find dependant Library
#
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH} )
FIND_PACKAGE(MED REQUIRED)
include_directories(${MED_INCLUDE_DIRS})

#
# Build the vtkMedReader
#
add_subdirectory(IO)

#
# Build the parallel vtkPMedReader
#
set(MedReader_BUILD_PARALLEL ${VTK_USE_PARALLEL} CACHE BOOL "Build the Med Reader with parallel support.")

#
# Build the ParaView Plugin
#
if(MedReader_BUILD_ParaView_Plugin)
	add_subdirectory(ParaViewPlugin)
endif(MedReader_BUILD_ParaView_Plugin)


	
