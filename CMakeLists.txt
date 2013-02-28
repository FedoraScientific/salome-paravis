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

PROJECT(PARAVIS)

IF(WINDOWS)
  STRING( REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} )
  SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${replacementFlags}" )
ENDIF(WINDOWS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)            

SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "KERNEL_ROOT_DIR")
SET(MODULE PARAVIS)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindPLATFORM.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindPYTHON.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindOMNIORB.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindPTHREADS.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindHDF5.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindBOOST.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindLIBXML2.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindSWIG.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindCPPUNIT.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindDOXYGEN.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/FindKERNEL.cmake)

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindPARAVIEW.cmake)
IF(EXISTS ${PARAVIEW_VTK_DIR}/VTKConfig.cmake)
  SET(VTK_DIR ${PARAVIEW_VTK_DIR})
ENDIF(EXISTS ${PARAVIEW_VTK_DIR}/VTKConfig.cmake)

SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "GUI_ROOT_DIR")
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindCAS.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindQT4.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindOPENGL.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindVTK.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindQWT.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindSIPPYQT.cmake)
INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/deprecated/FindGUI.cmake)

SET(VERSION_MAJOR       7)
SET(VERSION_MINOR       0)
SET(VERSION_MAINTENANCE 0)
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MAINTENANCE}")
SET(XVERSION 0x0${VERSION_MAJOR}0${VERSION_MINOR}0${VERSION_MAINTENANCE})
SET(VERSION_DEV 1)

SET(BUILD_PLUGINS 1)

SET(VISU_ROOT_DIR $ENV{VISU_ROOT_DIR} CACHE PATH "VISU_ROOT_DIR")
IF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
  INCLUDE(${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
  SET(WITH_VISU 1)
ENDIF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)

SET(MED_ROOT_DIR $ENV{MED_ROOT_DIR} CACHE PATH "MED_ROOT_DIR")
INCLUDE(${MED_ROOT_DIR}/adm_local/cmake_files/FindMED.cmake)

EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/getwrapclasses.py ${PARAVIEW_INCLUDE_DIRS})

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

ADD_DEFINITIONS(-DVTK_EXCLUDE_STRSTREAM_HEADERS)

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
