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

SET(_PYFILES_TO_INSTALL
  modes.py
  )

# Add warning message into scripts to be installed
INSTALL(CODE "
  SET(warning_str \"\# DO NOT MAKE CHANGES to this file, it will be rewrote 
\# on activation of SALOME PARAVIS module.
\# Modify the source file instead.\")

  FOREACH(file ${_PYFILES_TO_INSTALL})
    SET(tmp_file tmp/\${file})
    FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/\${file} tmp_string)
    STRING(REPLACE \"\# Do not touch this anchor comment\" \"\${warning_str}\" new_tmp_string \${tmp_string})
    FILE(WRITE \${tmp_file} \${new_tmp_string})
  ENDFOREACH()
")

# Install of the modified scripts
FOREACH(file ${_PYFILES_TO_INSTALL})
  INSTALL_AND_COMPILE_PYTHON_FILE(${CMAKE_CURRENT_BINARY_DIR}/tmp/\${file} ${SALOME_INSTALL_SCRIPT_PYTHON}/Macro)
ENDFOREACH()

SET(MAKE_CLEAN_FILES tmp)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${MAKE_CLEAN_FILES}")
