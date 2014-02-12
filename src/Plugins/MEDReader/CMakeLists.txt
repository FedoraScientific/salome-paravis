# Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

# Patch for ParaView
# cp ParaView-3.98.1/VTK/CMake/*.cmake ParaView-3.98.1_INSTALL/lib/cmake/paraview-3.98

# Env for MEDReader
#setenv PV_PLUGIN_PATH ".../MEDReader_INSTALL/lib/paraview"
#setenv LD_LIBRARY_PATH ...MEDReader_INSTALL/lib/salome:${LD_LIBRARY_PATH}

# Typical cmake command
# env MED_ROOT_DIR=... cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=PATH_TO/MEDReader_INSTALL ../MEDReader_SRC -DMEDFILE_ROOT_DIR=/where/is/med-3.0.7_install

PROJECT(MEDReader)
ENABLE_TESTING()
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(MED_READER_MAJOR_VERSION 0)
SET(MED_READER_MINOR_VERSION 0)
SET(MED_READER_BUILD_VERSION 0)
SET(MED_READER_VERSION "${MED_READER_MAJOR_VERSION}.${MED_READER_MINOR_VERSION}.${MED_READER_BUILD_VERSION}")

#
# Load the ParaView/VTK packages
#
SET(MEDReader_BUILD_ParaView_Plugin ON CACHE BOOL "Build the ParaView plugin")

IF(MEDReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(ParaView REQUIRED)
    IF(NOT ParaView_FOUND)
        MESSAGE(FATAL_ERROR "Please locate ParaView." )
    ENDIF(NOT ParaView_FOUND)
    INCLUDE(${PARAVIEW_USE_FILE})
    # Load information about VTK/ParaView modules loaded/available.
    PV_SETUP_MODULE_ENVIRONMENT("vtkMEDReader")
ELSE(MEDReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(VTK REQUIRED)
    if(NOT VTK_FOUND)
        MESSAGE(FATAL_ERROR "Please locate VTK." )
    ENDIF(NOT VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
ENDIF(MEDReader_BUILD_ParaView_Plugin)
 
# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." ${VTK_BUILD_SHARED_LIBS})
#
# Find dependant Library
#
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "KERNEL_ROOT_DIR")
SET(MED_ROOT_DIR $ENV{MED_ROOT_DIR} CACHE PATH "MED_ROOT_DIR")
#LIST(APPEND CMAKE_MODULE_PATH "${MED_ROOT_DIR}/adm_local_without_kernel/cmake_files")
LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
LIST(APPEND CMAKE_MODULE_PATH "${MED_ROOT_DIR}/adm_local/cmake_files")
INCLUDE(SalomeMacros)
INCLUDE(SalomeSetupPlatform)
INCLUDE(SalomeMEDConfig)
FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED)
FIND_PACKAGE(SalomeMEDFile REQUIRED)

#
# process all vtk-modules defined in this plugin. This generates the necessary
# wrappings for all these modules as well.
#
SET(VTK_INSTALL_RUNTIME_DIR lib/salome)
SET(VTK_INSTALL_LIBRARY_DIR lib/salome)
SET(VTK_INSTALL_ARCHIVE_DIR lib/salome)
INCLUDE_DIRECTORIES(${MEDFILE_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
PV_PROCESS_MODULES()

#
# Build the parallel vtkPMEDReader
#
SET(MEDReader_BUILD_PARALLEL ${VTK_USE_PARALLEL} CACHE BOOL "Build the Med Reader with parallel support.")

#
# Build the ParaView Plugin
#
IF(MEDReader_BUILD_ParaView_Plugin)
  ADD_SUBDIRECTORY(ParaViewPlugin)
ENDIF(MEDReader_BUILD_ParaView_Plugin)

ADD_SUBDIRECTORY(Test)
