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

## Parser for VTK header files 
## ===========================
##
## Everything under the ParaView subdirectory is a pure copy of ParaView source
## code.

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/ParaView
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/PVGUI
  )
  
# Wrapper executable generating the headers, source files and IDL files 
# corresponding to VTK wrapping:
SET(_wrap_PROGRAMS
  vtkWrapIDL_HH
  vtkWrapIDL_CC
  vtkWrapIDL
  )

SET(vtkWrapIDL_HH_CPPFLAGS -DIDL_I_HH)
SET(vtkWrapIDL_CC_CPPFLAGS -DIDL_I_CC)
SET(vtkWrapIDL_CPPFLAGS)

#VTN TODO: vtkParseData supports two types of structures for wrapping.
#We use old structures currently, but is is not influent for results of wrapping.
#It may be necessary to port vtkWrapIDL to new approach in future.
#IF(VTK_LEGACY_REMOVE)  
#  ADD_DEFINITIONS(-DVTK_PARSE_LEGACY_REMOVE)
#ENDIF()

FOREACH(name ${_wrap_PROGRAMS})
  ADD_EXECUTABLE(${name} 
    ParaView/vtkParse.tab.c 
    ParaView/vtkParsePreprocess.c
    ParaView/vtkParseHierarchy.c
    ParaView/vtkParseExtras.c
    ParaView/vtkParseMain.c
    ParaView/vtkParseData.c
    ParaView/vtkParseString.c
    ParaView/vtkWrap.c
#    ParaView/vtkWrapText.c
    ${PROJECT_SOURCE_DIR}/idl/vtkWrapIDL.c)
  ADD_DEPENDENCIES(${name} generate_txt)
  
  set_property(SOURCE ParaView/vtkParseMain.c PROPERTY COMPILE_DEFINITIONS
    VTK_PARSE_VERSION="${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")

  SET(flags)
  FOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${name}_CPPFLAGS})
    SET(flags "${flags} ${f}")
  ENDFOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${name}_CPPFLAGS})
  
  SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
  TARGET_LINK_LIBRARIES(${name} ${PLATFORM_LIBADD})
  
  SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${name})
  IF(WINDOWS)
    IF(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe)
    ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${name}.exe)
    ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
  ENDIF(WINDOWS)
  INSTALL(FILES ${${name}_EXEFILE} DESTINATION bin/salome PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDFOREACH(name ${_wrap_PROGRAMS})

# Generate headers and sources corresponding to the IDL implementations:
INCLUDE(wrap.cmake)

## From 2.8.10, we should use:
## SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ADD_LIBRARY(VTK_IDL_Impl OBJECT ${WRAP_IDL_I_CC})
# Ensure .hh files corresponding to IDL have been generated first:
ADD_DEPENDENCIES(VTK_IDL_Impl omniorb_module_SalomeIDLPARAVIS)

# Make the list of wrapped files visible from the top directory (will be used in PVGUI and idl):
SET(WRAP_IDL_I_HH "${WRAP_IDL_I_HH}" PARENT_SCOPE)
SET(WRAP_IDL_I_CC "${WRAP_IDL_I_CC}" PARENT_SCOPE)
