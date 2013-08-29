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

IF (PYTHON_EXECUTABLE)
  FOREACH ( tfile
      A0
      A1
      A2
      A3
      A4
      A5
      A6
      A7
      A9
      B1
      C3
      C4
      C5
      C6
      C7
      C8
      C9
      D0
      D1
      D3
      D5
      D6
      D7
      E0
      )
    SET(TIMEOUT 10000)
    ADD_TEST(BUGS_${tfile} ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/VisuPrs/Util/paravistesthelper.py ${TIMEOUT} ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py ${PARAVIS_TEST_OUTPUT_DIR})
    SET_TESTS_PROPERTIES(BUGS_${tfile} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILED" TIMEOUT ${TIMEOUT})
  ENDFOREACH( tfile )
ENDIF (PYTHON_EXECUTABLE)
