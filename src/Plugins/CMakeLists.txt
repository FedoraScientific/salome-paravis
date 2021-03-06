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

INCLUDE(${PARAVIEW_USE_FILE})

IF(WIN32)
  REMOVE_DEFINITIONS(-D_SECURE_SCL=0 -D_SECURE_SCL_THROWS=0)
  IF(NOT CMAKE_BUILD_TYPE STREQUAL "RELEASE" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    REMOVE_DEFINITIONS(-D_HAS_ITERATOR_DEBUGGING=0)  
  ENDIF() 
ENDIF() 

SET(_subdirs
  MEDReader 
#  TableReader  
  ElevationSurface
  ScaleVector
  EllipseBuilder
  )
  
IF(NOT SALOME_LIGHT_ONLY)
  LIST(APPEND _subdirs
    ParaMEDCorba 
  )
ENDIF()   

FOREACH(_dir ${_subdirs})
  ADD_SUBDIRECTORY(${_dir})
ENDFOREACH()
