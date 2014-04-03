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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)
PROJECT(SalomePARAVIS C CXX)

IF(WIN32)
  STRING( REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} )
  SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${replacementFlags}" )
ENDIF(WIN32)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 7)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 3)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 1)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
SET(${PROJECT_NAME_UC}_VERSION_DEV 1)

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)   # From KERNEL
# Always build libraries as shared objects:
SET(BUILD_SHARED_LIBS TRUE)
# Local macros:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm_local/cmake_files")

# Detect GUI root dir (but doesn't find GUI yet! See below)
# ===================
SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "Path to the Salome GUI")
IF(EXISTS ${GUI_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${GUI_ROOT_DIR}/adm_local/cmake_files")
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need a Salome GUI, please define GUI_ROOT_DIR")
ENDIF()

# User options
# ============
OPTION(SALOME_BUILD_DOC "Generate SALOME GUI documentation" ON)
OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ON)
OPTION(SALOME_PARAVIS_ALL_TEST "Add all tests for Salome PARAVIS module" OFF)
OPTION(SALOME_PARAVIS_BUILD_PLUGINS "Build PARAVIS plugins (MEDReader, etc ...)" ON)
CMAKE_DEPENDENT_OPTION(SALOME_LIGHT_ONLY "Build SALOME Light only (no CORBA)" OFF
                       "NOT SALOME_KERNEL_LIGHT_ONLY" ON)
CMAKE_DEPENDENT_OPTION(SALOME_PARAVIS_MINIMAL_CORBA "Build PARAVIS with a minimal CORBA interface" OFF
                       "NOT SALOME_LIGHT_ONLY" ON)

IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
ENDIF()

# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can help finding the smaller prerequisites and detect conficts.
# In our case KERNEL has already loaded many prereq which are also used in GUI:

##
## From KERNEL:
##

FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)

IF(NOT SALOME_LIGHT_ONLY)
  FIND_PACKAGE(SalomeOmniORB REQUIRED)
  FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
ENDIF()

IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen)
  FIND_PACKAGE(SalomeGraphviz)
  FIND_PACKAGE(SalomeSphinx)
  SALOME_LOG_OPTIONAL_PACKAGE(Doxygen SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Graphviz SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Sphinx SALOME_BUILD_DOC)
ENDIF()

##
## From GUI: part 1 (Qt must be checked before ParaView)
##
FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtHelp)
# This search for qhelpgenerator is required here
# to avoid detecting native one due to bug in ParaViewMacros.cmake
FIND_PROGRAM(QT_HELP_GENERATOR
    qhelpgenerator
    PATHS "${QT_BINARY_DIR}"
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
    DOC "qhelpgenerator used to compile Qt help project files")
MARK_AS_ADVANCED(QT_HELP_GENERATOR)

##
## Specific to ParaViS:
##
FIND_PACKAGE(SalomeParaView REQUIRED)

# GUI itself has to be loaded AFTER ParaView: the ParaView config doesn't mix
# well with the VTK one (and GUI loads VTK):
FIND_PACKAGE(SalomeGUI REQUIRED)
FULL_GUI(TRUE) # check whether GUI builded in full mode and with CORBA
ADD_DEFINITIONS(${GUI_DEFINITIONS})
INCLUDE_DIRECTORIES(${GUI_INCLUDE_DIRS})

##
## From GUI: part 2
##
FIND_PACKAGE(SalomeCAS REQUIRED)

##
## From MED
##
IF(SALOME_PARAVIS_BUILD_PLUGINS)
  SET(MED_ROOT_DIR $ENV{MED_ROOT_DIR} CACHE PATH "Path to the Salome MED module")
  IF(EXISTS ${MED_ROOT_DIR})
    LIST(APPEND CMAKE_MODULE_PATH "${MED_ROOT_DIR}/adm_local/cmake_files")
    FIND_PACKAGE(SalomeMED REQUIRED)
  ELSE()
    MESSAGE(FATAL_ERROR "We absolutely need the Salome MED module to build plugins, please define MED_ROOT_DIR")
  ENDIF()
  IF(SALOME_MED_STANDALONE OR SALOME_MED_MICROMED)
    MESSAGE(FATAL_ERROR "We absolutely need a Salome MED with KERNEL and with MED file dependancy, 
                         please set SALOME_MED_STANDALONE=OFF and SALOME_MED_MICROMED=OFF in MED module
                         or SALOME_PARAVIS_BUILD_PLUGINS=OFF in PARAVIS module")
  ENDIF()
  FIND_PACKAGE(SalomeMEDFile)
  SALOME_LOG_OPTIONAL_PACKAGE(MEDFile SALOME_PARAVIS_BUILD_PLUGINS)
ENDIF()

## Detection summary:
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
# (default values taken from KERNEL)
# ===========
SET(SALOME_INSTALL_BINS "${SALOME_INSTALL_BINS}" CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS "${SALOME_INSTALL_LIBS}" CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_IDLS "${SALOME_INSTALL_IDLS}" CACHE PATH "Install path: SALOME IDL files")
SET(SALOME_INSTALL_HEADERS "${SALOME_INSTALL_HEADERS}" CACHE PATH "Install path: SALOME headers")
SET(SALOME_INSTALL_SCRIPT_SCRIPTS "${SALOME_INSTALL_SCRIPT_SCRIPTS}" CACHE PATH 
   "Install path: SALOME scripts")
SET(SALOME_INSTALL_SCRIPT_DATA "${SALOME_INSTALL_SCRIPT_DATA}" CACHE PATH 
   "Install path: SALOME script data")
SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
   "Install path: SALOME Python scripts")
SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
   "Install path: SALOME Python shared modules")
SET(SALOME_INSTALL_CMAKE "${SALOME_INSTALL_CMAKE}" CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL "${SALOME_INSTALL_CMAKE_LOCAL}" CACHE PATH 
    "Install path: local SALOME CMake files") 
SET(SALOME_INSTALL_AMCONFIG_LOCAL "${SALOME_INSTALL_AMCONFIG_LOCAL}" CACHE PATH
  "Install path: local SALOME config files (obsolete, to be removed)")

SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")
SET(SALOME_INSTALL_DOC "${SALOME_INSTALL_DOC}" CACHE PATH "Install path: SALOME documentation")

# Specific to PARAVIS:
SET(SALOME_PARAVIS_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/paravis" CACHE PATH "Install path: SALOME PARAVIS specific data")

MARK_AS_ADVANCED(SALOME_INSTALL_BINS SALOME_INSTALL_LIBS SALOME_INSTALL_IDLS SALOME_INSTALL_HEADERS)
MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_SCRIPTS SALOME_INSTALL_SCRIPT_DATA SALOME_INSTALL_SCRIPT_PYTHON)
MARK_AS_ADVANCED(SALOME_INSTALL_APPLISKEL_SCRIPTS  SALOME_INSTALL_APPLISKEL_PYTHON SALOME_INSTALL_CMAKE SALOME_INSTALL_CMAKE_LOCAL SALOME_INSTALL_RES)
MARK_AS_ADVANCED(SALOME_INSTALL_PYTHON SALOME_INSTALL_PYTHON_SHARED)
MARK_AS_ADVANCED(SALOME_INSTALL_AMCONFIG_LOCAL SALOME_INSTALL_DOC SALOME_PARAVIS_INSTALL_RES_DATA)

## Generation of the list of VTK classes (to be wrapped into IDLs) 
## =====================================
IF(NOT SALOME_LIGHT_ONLY AND NOT SALOME_PARAVIS_MINIMAL_CORBA)
  ADD_DEFINITIONS(-DPARAVIS_WITH_FULL_CORBA)
  # TODO: the below requires ParaView in the PYTHONPATH ... not so nice:
  MESSAGE(STATUS "Generating list of wrapped VTK classes ...")
  LIST(GET PARAVIEW_INCLUDE_DIRS 0 PARAVIEW_INCLUDE_DIR0)
  EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/VTKWrapping/getwrapclasses.py ${PARAVIEW_INCLUDE_DIR0}
      RESULT_VARIABLE _res)
  IF(NOT _res EQUAL 0)
    MESSAGE(FATAL_ERROR "Unable to run the Python script retrieving the list of VTK classes.")
  ENDIF()
ENDIF()

# Accumulate environment variables for PARAVIS module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS}) 

# Sources 
# ========
IF(NOT SALOME_LIGHT_ONLY)
  ADD_SUBDIRECTORY(idl)   # Order matters when using the wrapping - to be added AFTER src
ENDIF()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(adm_local)
ADD_SUBDIRECTORY(resources)

IF(SALOME_BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF()
IF(SALOME_BUILD_TESTS AND NOT SALOME_LIGHT_ONLY)
  ADD_SUBDIRECTORY(test)
ENDIF()
ADD_SUBDIRECTORY(bin)

# Header configuration
# ====================
SALOME_XVERSION(${PROJECT_NAME})
SALOME_CONFIGURE_FILE(PARAVIS_version.h.in PARAVIS_version.h INSTALL ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
IF(NOT SALOME_LIGHT_ONLY)
  SET(_${PROJECT_NAME}_exposed_targets SalomeIDLPARAVIS PARAVISEngine PARAVIS)

  # Add all targets to the build-tree export set
  EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
    FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
ENDIF()

# Create the configuration files:
#   - in the build tree:

# Ensure the variables are always defined for the configure:
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(GUI_ROOT_DIR "${GUI_ROOT_DIR}")
SET(MED_ROOT_DIR "${MED_ROOT_DIR}")
SET(MEDFILE_ROOT_DIR "${MEDFILE_ROOT_DIR}")
 
SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
SALOME_CONFIGURE_PREPARE(MEDFile)

CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in 
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
    KERNEL_ROOT_DIR GUI_ROOT_DIR MED_ROOT_DIR MEDFILE_ROOT_DIR)

#   - in the install tree (VSR 16/08/2013: TEMPORARILY COMMENT THIS - TO REMOVE?):
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
#SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
#CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
#    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
#    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
#    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX 
#    KERNEL_ROOT_DIR GUI_ROOT_DIR MED_ROOT_DIR MEDFILE_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

IF(NOT SALOME_LIGHT_ONLY)
  # Install the export set for use with the install-tree
  INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
          FILE ${PROJECT_NAME}Targets.cmake)
ENDIF()
