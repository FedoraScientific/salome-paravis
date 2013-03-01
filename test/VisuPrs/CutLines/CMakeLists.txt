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
      A1
      A2
      A3
      A4
      A5
      A6      
      A7
      A8
      A9
      B0
      B1
      B2
      B3
      E0
      E1
      E2
      E3      
      E4
      E5
      E6
      E7
      E8
      E9
      F1
      F2
      F3
      F4
      F5
      F6 
      F7
      F8
      F9
      G0     
      G1
      G2      
      )
    SET(TIMEOUT 10000)
    ADD_TEST(CUTLINES_${tfile} ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/VisuPrs/Util/paravistesthelper.py ${TIMEOUT} ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py ${PARAVIS_TEST_OUTPUT_DIR})
    SET_TESTS_PROPERTIES(CUTLINES_${tfile} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILED" TIMEOUT ${TIMEOUT})
  ENDFOREACH( tfile )
ENDIF (PYTHON_EXECUTABLE)
