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

SET(VERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MAINTENANCE})
SET(SOVERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR})

ADD_DEFINITIONS(-DHAVE_SALOME_CONFIG)
INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIRS} ${KERNEL_ROOT_DIR}/include/salome ${CMAKE_BINARY_DIR}/idl)

INCLUDE(wrap.cmake)


SET(BASEIDL_FILES
  PARAVIS_Gen_Types.idl
  PARAVIS_Gen.idl
  )

SET(BASEIDL_SK_FILES
  PARAVIS_Gen_TypesSK.cc
  PARAVIS_GenSK.cc
  )

# potential problem on parallel make on the following - multiple outputs
SET(SUFFIXES
  .idl
  .hh
  SK.cc
  )

SET(srcs)
FOREACH(f ${BASEIDL_SK_FILES} ${WRAP_SK_FILES})
  SET(srcs ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/${f})
ENDFOREACH(f ${BASEIDL_SK_FILES} ${WRAP_SK_FILES})

IF(WINDOWS)
ADD_LIBRARY(SalomeIDLPARAVIS STATIC ${srcs})
ELSE(WINDOWS)
ADD_LIBRARY(SalomeIDLPARAVIS SHARED ${srcs})
ENDIF(WINDOWS)

SET(flags)
FOREACH(f ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${OMNIORB_DEFINITIONS} ${KERNEL_CXXFLAGS})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${OMNIORB_DEFINITIONS} ${KERNEL_CXXFLAGS})

SET_TARGET_PROPERTIES(SalomeIDLPARAVIS PROPERTIES COMPILE_FLAGS "${flags}")
SET_TARGET_PROPERTIES(SalomeIDLPARAVIS PROPERTIES VERSION ${VERSION_INFO} SOVERSION ${SOVERSION_INFO})

TARGET_LINK_LIBRARIES(SalomeIDLPARAVIS ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${OMNIORB_LIBRARIES})
INSTALL(TARGETS SalomeIDLPARAVIS DESTINATION lib/salome)
  
ADD_EXECUTABLE(vtkWrapIDL 
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParse.tab.c 
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParsePreprocess.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseHierarchy.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseExtras.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseMain.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseData.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseString.c
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkWrap.c
#  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkWrapText.c
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkWrapIDL.c)

set_property(SOURCE ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParseMain.c PROPERTY COMPILE_DEFINITIONS
  VTK_PARSE_VERSION="${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")

ADD_DEPENDENCIES(vtkWrapIDL generate_txt)
  
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/ParaView)
SET(flags)
FOREACH(f ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${PARAVIEW_INCLUDES} ${VTK_INCLUDES})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${PARAVIEW_INCLUDES} ${VTK_INCLUDES})

#VTN TODO: vtkParseData supports two types of structures for wrapping.
#We use old structures currently, but is is not influent for results of wrapping.
#It may be necessary to port vtkWrapIDL to new approach in future.
#IF(VTK_LEGACY_REMOVE)
#  ADD_DEFINITIONS(-DVTK_PARSE_LEGACY_REMOVE)
#ENDIF()
SET_TARGET_PROPERTIES(vtkWrapIDL PROPERTIES COMPILE_FLAGS "${flags}")

TARGET_LINK_LIBRARIES(vtkWrapIDL ${PLATFORM_LDFLAGS} ${PLATFORM_LIBADD})
SET(vtkWrapIDL_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL)
IF(WINDOWS)
  IF(CMAKE_BUILD_TOOL STREQUAL nmake)
    SET(vtkWrapIDL_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL.exe)
  ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
    SET(vtkWrapIDL_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/vtkWrapIDL.exe)
  ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
ENDIF(WINDOWS)
INSTALL(FILES ${vtkWrapIDL_EXEFILE} DESTINATION bin/salome PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

FOREACH(input ${BASEIDL_FILES} ${WRAP_IDL})
  STRING(REGEX REPLACE ".idl" "" base ${input})
  SET(src ${CMAKE_CURRENT_BINARY_DIR}/${base}SK.cc)
  SET(outputs ${src})
  SET(dynsrc ${CMAKE_CURRENT_BINARY_DIR}/${base}DynSK.cc)
  SET(outputs ${outputs} ${dynsrc})
  SET(inc ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
  SET(outputs ${outputs} ${inc})
  LIST(FIND BASEIDL_FILES ${input} BASE_INDEX)
  IF(${BASE_INDEX} EQUAL -1)
    SET(input ${CMAKE_CURRENT_BINARY_DIR}/${input})
  ELSE(${BASE_INDEX} EQUAL -1)
    SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  ENDIF(${BASE_INDEX} EQUAL -1)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${outputs}
    COMMAND ${OMNIORB_IDL_PYTHON} -bcxx ${IDLCXXFLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -I${KERNEL_ROOT_DIR}/idl/salome ${OMNIORB_IDLCXXFLAGS} ${input}
    MAIN_DEPENDENCY ${input}
    )
  INSTALL(FILES ${input} DESTINATION idl/salome)

  SET(IDL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
  INSTALL(FILES ${IDL_HEADER} DESTINATION include/salome)
  INSTALL(CODE "SET(IDL_FILE ${input})")
  INSTALL(CODE "SET(DIR lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/salome)")
  INSTALL(CODE "SET(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})")
  INSTALL(CODE "SET(OMNIORB_IDL_PYTHON ${OMNIORB_IDL_PYTHON})")
  INSTALL(CODE "SET(IDLPYFLAGS -bpythonIDL -p${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${KERNEL_ROOT_DIR}/idl/salome)")
  INSTALL(SCRIPT ${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/install_python_from_idl.cmake)

ENDFOREACH(input ${IDL_FILES})
