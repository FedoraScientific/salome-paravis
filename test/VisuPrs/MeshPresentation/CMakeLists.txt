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

IF(SALOME_PARAVIS_ALL_TEST)
  SET(TEST_NAMES A0 A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 B0 B1 B2 E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 
                 F1 F2 F3 F4 F5 F6 F7 F8 F9 G0 G1 G3 G4 G5 G6 G7 G8 G9 
                 H0 H1 H2 H3 H4 H5 H6 H7 H8 H9 I0 I1 I2 I3 I4 I5 I6 I7 I8 I9 
                 J0 J1 J2 J3 J4 J5 J6 J7 J8 J9 K0 K1 K2 K3 K4 K5 K6 K7 K8 K9 L0 L1)
ELSE()
  SET(TEST_NAMES A0 B0 E2 F2 G3 H1 I0 J0 K1 L0)
ENDIF()

FOREACH(tfile ${TEST_NAMES})
  ADD_TEST(MESH_${tfile} ${PYTHON_EXECUTABLE} ${PARAVIS_TEST_HELPER_FILE} 
                    ${TIMEOUT} ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py)
  SET_TESTS_PROPERTIES(MESH_${tfile} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILED" TIMEOUT ${TIMEOUT})
  SET_TESTS_PROPERTIES(MESH_${tfile} PROPERTIES ENVIRONMENT "${tests_env}")
ENDFOREACH()











