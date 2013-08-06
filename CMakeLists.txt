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

PROJECT(PARAVIS C CXX)

IF(WINDOWS)
  STRING( REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} )
  SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${replacementFlags}" )
ENDIF(WINDOWS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7 FATAL_ERROR)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)            

SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "KERNEL_ROOT_DIR")
SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "GUI_ROOT_DIR")
SET(MODULE PARAVIS)

SET(CMAKE_MODULE_PATH 
    "${KERNEL_ROOT_DIR}/salome_adm/cmake_files" 
    "${GUI_ROOT_DIR}/adm_local/cmake_files"
    "${PROJECT_SOURCE_DIR}/adm_local/cmake_files")

INCLUDE(SalomeMacros)
INCLUDE(SalomeSetupPlatform)

FIND_PACKAGE(SalomeKERNEL REQUIRED)

# From KERNEL:
FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomeOmniORB REQUIRED)
FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
SET(OMNIORB_IDL_PYTHON ${OMNIORB_IDL_COMPILER}) 
FIND_PACKAGE(SalomePThread REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED)
FIND_PACKAGE(SalomeBoost REQUIRED)
FIND_PACKAGE(SalomeLibXml2 REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)
FIND_PACKAGE(SalomeCppUnit REQUIRED)
FIND_PACKAGE(SalomeDoxygen REQUIRED)

# From GUI: part 1 (Qt must be checked before ParaView)
FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtHelp)
# This search for qhelpgenerator is required here
# to avoid detecting native one due to bug in ParaViewMacros.cmake
FIND_PROGRAM(QT_HELP_GENERATOR
    qhelpgenerator
    PATHS "${QT_BINARY_DIR}"
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
    DOC "qhelpgenerator used to compile Qt help project files")
MARK_AS_ADVANCED(QT_HELP_GENERATOR)

# Specific to ParaVis:
FIND_PACKAGE(SalomeParaView)
IF(TARGET vtkTools)
 message("bli !!")
endif()

# GUI has to be loaded AFTER ParaView: the ParaView config doesn't mix
# well with the VTK one (and GUI loads VTK):
FIND_PACKAGE(SalomeGUI REQUIRED)

# From GUI: part 2
FIND_PACKAGE(SalomeCAS REQUIRED)
FIND_PACKAGE(SalomeOpenGL REQUIRED)
FIND_PACKAGE(SalomeVTK REQUIRED)
FIND_PACKAGE(SalomeQwt REQUIRED)
FIND_PACKAGE(SalomeSIP REQUIRED)
FIND_PACKAGE(SalomePyQt4 REQUIRED)

SET(VERSION_MAJOR 7)
SET(VERSION_MINOR 2)
SET(VERSION_MAINTENANCE 0)
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MAINTENANCE}")
SET(XVERSION 0x0${VERSION_MAJOR}0${VERSION_MINOR}0${VERSION_MAINTENANCE})
SET(VERSION_DEV 0)

SET(BUILD_PLUGINS 1)

#SET(VISU_ROOT_DIR $ENV{VISU_ROOT_DIR} CACHE PATH "VISU_ROOT_DIR")
#IF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
  #INCLUDE(${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
  #SET(WITH_VISU 1)
#ENDIF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)

SET(MED_ROOT_DIR "$ENV{MED_ROOT_DIR}" CACHE PATH "Path to the MED Salome module")
LIST(APPEND CMAKE_MODULE_PATH "${MED_ROOT_DIR}/adm_local/cmake_files")
FIND_PACKAGE(SalomeMED REQUIRED)
FIND_PACKAGE(SalomeMEDFile REQUIRED)

# TODO: the below requires ParaView in the PYTHONPATH ... not so nice:
MESSAGE(STATUS "Generating wrapped class list (??) ...")
LIST(GET PARAVIEW_INCLUDE_DIRS 0 PARAVIEW_INCLUDE_DIR0)
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/getwrapclasses.py ${PARAVIEW_INCLUDE_DIR0}
    RESULT_VARIABLE _res)
IF(NOT _res EQUAL 0)
  MESSAGE(FATAL_ERROR "Unable to run the Python script retrieving the list of VTK classes.")
ENDIF()

SET(SUBDIRS
  idl
  adm_local
  resources
  src
  doc
  bin
  )

SET(DISTCLEANFILES
  wrapfiles.txt
  )

INCLUDE(wrapfiles.cmake)

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/PARAVIS_version.h.in)
SET(output ${CMAKE_CURRENT_BINARY_DIR}/PARAVIS_version.h)

MESSAGE(STATUS "Creation of ${output}")
CONFIGURE_FILE(${input} ${output})

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})

# Reset CMAKE_CONFIGURATION_TYPES to the good value defined by CMAKE_BUILD_TYPE
IF(CMAKE_BUILD_TYPE)
  IF(WINDOWS)
    MARK_AS_ADVANCED(CLEAR CMAKE_CONFIGURATION_TYPES)
    SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "compilation types" FORCE)
  ENDIF(WINDOWS)
ENDIF(CMAKE_BUILD_TYPE)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/PARAVIS_version.h DESTINATION include/salome)

# Configure Testing
SET(PARAVIS_TEST_OUTPUT_DIR /tmp/pic CACHE PATH "Directory for saving test pictures.")
OPTION(BUILD_TESTING "Build ParaVis Testing" ON)
IF(BUILD_TESTING)
   SET(PARAVIS_TEST_DIR ${ParaVis_BINARY_DIR}/Test/Temporary)
   MAKE_DIRECTORY(${PARAVIS_TEST_DIR})
   ENABLE_TESTING()
   INCLUDE (CTest)
   ADD_SUBDIRECTORY(test)
ENDIF(BUILD_TESTING)
