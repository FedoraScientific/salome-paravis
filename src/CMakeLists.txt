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

SET(_subdirs
  ENGINE
  PVGUI
  PV_SWIG
  Macro
  )
  
IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
  # Append in first position  
  LIST(INSERT _subdirs 0 VTKWrapping)
  ADD_DEFINITIONS(-DPARAVIS_WITH_FULL_CORBA)
ENDIF()

IF(SALOME_PARAVIS_BUILD_PLUGINS)
  LIST(APPEND _subdirs Plugins)
ENDIF()

FOREACH(dir ${_subdirs})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH()

SET(WRAP_IDL_I_HH "${WRAP_IDL_I_HH}" PARENT_SCOPE)
SET(WRAP_IDL_I_CC "${WRAP_IDL_I_CC}" PARENT_SCOPE)

