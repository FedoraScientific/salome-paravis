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

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
ADD_TEST(testMEDReader0 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader0.py)
SET_TESTS_PROPERTIES(testMEDReader0 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader1 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader1.py)
SET_TESTS_PROPERTIES(testMEDReader1 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader2 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader2.py)
SET_TESTS_PROPERTIES(testMEDReader2 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader3 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader3.py)
SET_TESTS_PROPERTIES(testMEDReader3 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader4 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader4.py)
SET_TESTS_PROPERTIES(testMEDReader4 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader5 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader5.py)
SET_TESTS_PROPERTIES(testMEDReader5 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader6 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader6.py)
SET_TESTS_PROPERTIES(testMEDReader6 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader7 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader7.py)
SET_TESTS_PROPERTIES(testMEDReader7 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader8 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader8.py)
SET_TESTS_PROPERTIES(testMEDReader8 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader9 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader9.py)
SET_TESTS_PROPERTIES(testMEDReader9 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader10 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader10.py)
SET_TESTS_PROPERTIES(testMEDReader10 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader11 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader11.py)
SET_TESTS_PROPERTIES(testMEDReader11 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader12 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader12.py)
SET_TESTS_PROPERTIES(testMEDReader12 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader13 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader13.py)
SET_TESTS_PROPERTIES(testMEDReader13 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader14 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader14.py)
SET_TESTS_PROPERTIES(testMEDReader14 PROPERTIES ENVIRONMENT "${tests_env}")
ADD_TEST(testMEDReader15 ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader15.py)
SET_TESTS_PROPERTIES(testMEDReader15 PROPERTIES ENVIRONMENT "${tests_env}")
