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

LIST(GET PARAVIEW_INCLUDE_DIRS 0 PARAVIEW_INCLUDE_DIR0)

SET(WRAP_IDL_I_HH)
SET(WRAP_IDL_I_CC)

IF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)
 EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} -c "f = open('${CMAKE_BINARY_DIR}/wrapfiles.txt') ; print f.read(), ; f.close()"
  OUTPUT_VARIABLE WRAP_LIST_FULL
 )

 STRING(REGEX  MATCHALL "[^\n]+" WRAP_LIST_REG ${WRAP_LIST_FULL})
 FOREACH(_str ${WRAP_LIST_REG})

  SEPARATE_ARGUMENTS(_str)
  LIST(LENGTH _str WRAP_LEN)
  SET(DEP_HH)
  SET(DEP_CC)
 
  LIST(GET _str 0 VAL)

  IF(WRAP_LEN GREATER 1)
   MATH(EXPR WRAP_LEN1 "${WRAP_LEN} - 1" )

   FOREACH(IND RANGE 1 ${WRAP_LEN1})
    LIST(GET _str ${IND} DEP_VAL)
    SET(DEP_HH ${DEP_HH} PARAVIS_Gen_${DEP_VAL}_i.hh)
    SET(DEP_CC ${DEP_CC} PARAVIS_Gen_${DEP_VAL}_i.cc)
   ENDFOREACH(IND RANGE 1 ${WRAP_LEN1})

  ENDIF(WRAP_LEN GREATER 1)

  LIST(APPEND WRAP_IDL_I_HH PARAVIS_Gen_${VAL}_i.hh)
  LIST(APPEND WRAP_IDL_I_CC PARAVIS_Gen_${VAL}_i.cc)

  SET(vtkWrapIDL_HH_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL_HH)
  IF(WIN32)
    IF(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_HH_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL_HH.exe)
    ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_HH_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/${CMAKE_BUILD_TYPE}/vtkWrapIDL_HH.exe)
    ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
  ENDIF(WIN32)

  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_Gen_${VAL}_i.hh
   COMMAND ${vtkWrapIDL_HH_EXEFILE} -o PARAVIS_Gen_${VAL}_i.hh --hints ${CMAKE_BINARY_DIR}/idl/hints ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h
   DEPENDS vtkWrapIDL_HH ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints ${DEP_HH}
  ) 

  SET(vtkWrapIDL_CC_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL_CC)
  IF(WIN32)
    IF(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_CC_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/vtkWrapIDL_CC.exe)
    ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(vtkWrapIDL_CC_EXEFILE ${CMAKE_BINARY_DIR}/src/VTKWrapping/${CMAKE_BUILD_TYPE}/vtkWrapIDL_CC.exe)
    ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
  ENDIF(WIN32)

  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_Gen_${VAL}_i.cc
   COMMAND ${vtkWrapIDL_CC_EXEFILE} -o PARAVIS_Gen_${VAL}_i.cc --hints ${CMAKE_BINARY_DIR}/idl/hints ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h
   DEPENDS PARAVIS_Gen_${VAL}_i.hh vtkWrapIDL_CC ${PARAVIEW_INCLUDE_DIR0}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints ${DEP_CC}
  )

 ENDFOREACH(_str ${WRAP_LIST_REG})
ENDIF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)

ADD_CUSTOM_TARGET(generate_implements ALL DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt ${WRAP_IDL_I_HH} ${WRAP_IDL_I_CC})
