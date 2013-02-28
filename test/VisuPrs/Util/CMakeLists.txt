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

SET(dist_testscript_DATA
  paravistest.py
  )

FOREACH(f ${dist_testscript_DATA})
  INSTALL(FILES ${f} DESTINATION bin/salome)

  INSTALL(CODE "SET(PYTHON_FILE ${f})")
  INSTALL(CODE "SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})")
  INSTALL(CODE "SET(DEST bin/salome)")
  INSTALL(CODE "SET(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})")
  INSTALL(SCRIPT ${KERNEL_ROOT_DIR}/salome_adm/cmake_files/deprecated/install_and_compile_python_file.cmake)
  
ENDFOREACH(f ${dist_testscript_DATA})
