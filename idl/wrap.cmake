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

LIST(GET PARAVIEW_INCLUDE_DIRS 0 PARAVIEW_INCLUDE_DIR0)

ADD_CUSTOM_COMMAND(
 OUTPUT vtkWrapIDL.h
 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_header.py ${CMAKE_BINARY_DIR}
 DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt
)

ADD_CUSTOM_COMMAND(
 OUTPUT hints
 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_hints.py ${PARAVIEW_LIBRARY_DIR0}
# DEPENDS ${PARAVIEW_LIBRARY_DIR0}/hints ${CMAKE_CURRENT_SOURCE_DIR}/hints_paravis
 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/hints ${CMAKE_CURRENT_SOURCE_DIR}/hints_paravis
)

SET(WRAP_IDL)
#SET(WRAP_SK_FILES)

IF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)
 EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} -c "f = open('${CMAKE_BINARY_DIR}/wrapfiles.txt') ; print f.read(), ; f.close()"
  OUTPUT_VARIABLE WRAP_LIST_FULL
 )

 STRING(REGEX  MATCHALL "[^\n]+" WRAP_LIST_REG ${WRAP_LIST_FULL})
 FOREACH(STR ${WRAP_LIST_REG})

  SEPARATE_ARGUMENTS(STR)
  LIST(LENGTH STR WRAP_LEN)
  SET(DEP)
 
  LIST(GET STR 0 VAL)

  IF(WRAP_LEN GREATER 1)
   MATH(EXPR WRAP_LEN1 "${WRAP_LEN} - 1" )

   FOREACH(IND RANGE 1 ${WRAP_LEN1})
    LIST(GET STR ${IND} DEP_VAL)
    SET(DEP ${DEP} PARAVIS_Gen_${DEP_VAL}.idl)
   ENDFOREACH(IND RANGE 1 ${WRAP_LEN1})

  ENDIF(WRAP_LEN GREATER 1)

  LIST(APPEND WRAP_IDL PARAVIS_Gen_${VAL}.idl)
  #SET(WRAP_SK_FILES ${WRAP_SK_FILES} PARAVIS_Gen_${VAL}SK.cc)
  SET(vtkWrapIDL_EXEFILE ${PROJECT_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL)
  IF(WIN32)
    IF(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_EXEFILE ${PROJECT_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL.exe)
    ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_EXEFILE ${PROJECT_BINARY_DIR}/src/VTKWrapping/${CMAKE_BUILD_TYPE}/vtkWrapIDL.exe)
    ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
  ENDIF(WIN32)
  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_Gen_${VAL}.idl
   COMMAND ${vtkWrapIDL_EXEFILE} -o PARAVIS_Gen_${VAL}.idl --hints hints ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h
   DEPENDS vtkWrapIDL ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h hints ${DEP}
  )

 ENDFOREACH(STR ${WRAP_LIST_REG})
ENDIF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)

ADD_CUSTOM_TARGET(generate_txt DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt vtkWrapIDL.h hints)
ADD_CUSTOM_TARGET(generate_idl ALL DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt vtkWrapIDL.h hints ${WRAP_IDL})

