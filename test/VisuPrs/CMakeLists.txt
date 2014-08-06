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

SET(TIMEOUT 10000)
SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

IF(NOT WIN32)
  # Special case for the PV_PLUGIN_PATH environment variable:
  # PV_PLUGIN_PATH=VAL_1;VAL_2;...;VAL_N
  # But the ';' character is used as separator between variables (PATH, LD_LIBRARY_PATH, etc...)
  # So we should shield a sequence ";/" by the '\' character.
  STRING(REGEX REPLACE ";/" "\\\\;/" tests_env "${tests_env}")
ENDIF(NOT WIN32)

SET(PARAVIS_TEST_HELPER_FILE ${PROJECT_SOURCE_DIR}/test/VisuPrs/Util/paravistesthelper.py)

SET(TEST_DIRECTORIES 
  Util
  2D_viewer
  3D_viewer
  ScalarMap
  DeformedShape
  ScalarMap_On_DeformedShape
  CutPlanes
  CutLines
  Vectors
  Plot3D
  IsoSurfaces
  MeshPresentation
  Animation
  GaussPoints
  StreamLines
  SWIG_scripts
  Tables
  ImportMedField
  united
  bugs
  imps
  dump_study
)
  
FOREACH(test_dir ${TEST_DIRECTORIES})
  ADD_SUBDIRECTORY(${test_dir})
ENDFOREACH()
