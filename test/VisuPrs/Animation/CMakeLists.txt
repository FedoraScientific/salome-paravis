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

IF (PYTHON_EXECUTABLE)
  FOREACH ( tfile
      A0
      A2
      A4
      A7
      A8
      B2
      B5
      B6  
      C0
      C1
      C3
      C8
      D1
      D2
      D6
      D9
      E0
      E4
      E7
      E8
      F2
      F5
      F6
      G0
      G3
      G4
      G8
      H1
      H2
      )
      ADD_TEST(ANIMATION_${tfile} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py ${PARAVIS_TEST_OUTPUT_DIR})
    SET_TESTS_PROPERTIES(ANIMATION_${tfile} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILED")
  ENDFOREACH( tfile )
ENDIF (PYTHON_EXECUTABLE)
