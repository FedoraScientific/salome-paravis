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

SET(dist_salomeres_DATA
  pqAppIcon22.png
  pqAppIcon16.png
  ParaViewFilters.xml
  ParaViewReaders.xml
  ParaViewSources.xml
  ParaViewWriters.xml
  )

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/PARAVISCatalog.xml.in)
SET(output ${CMAKE_CURRENT_BINARY_DIR}/PARAVISCatalog.xml)

MESSAGE(STATUS "Creation of ${output}")
CONFIGURE_FILE(${input} ${output})

INSTALL(FILES ${output} DESTINATION share/salome/resources/paravis)

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/SalomeApp.xml.in)
SET(output ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml)

MESSAGE(STATUS "Creation of ${output}")
CONFIGURE_FILE(${input} ${output})

INSTALL(FILES ${output} DESTINATION share/salome/resources/paravis)

FOREACH(f ${dist_salomeres_DATA})
  INSTALL(FILES ${f} DESTINATION share/salome/resources/paravis)
ENDFOREACH(f ${dist_salomeres_DATA})
