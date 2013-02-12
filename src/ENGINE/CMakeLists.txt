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

SET(VERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MAINTENANCE})
SET(SOVERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR})

ADD_DEFINITIONS(-DHAVE_SALOME_CONFIG)
INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR} ${KERNEL_ROOT_DIR}/include/salome ${CMAKE_BINARY_DIR}/idl)

ADD_LIBRARY(PARAVISEngine SHARED PARAVIS_Engine_i.cc)

SET(flags)
FOREACH(f ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${KERNEL_CXXFLAGS} ${OMNIORB_DEFINITIONS})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${var})

SET_TARGET_PROPERTIES(PARAVISEngine PROPERTIES COMPILE_FLAGS "${flags}")
SET_TARGET_PROPERTIES(PARAVISEngine PROPERTIES VERSION ${VERSION_INFO} SOVERSION ${SOVERSION_INFO})

TARGET_LINK_LIBRARIES(PARAVISEngine ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${KERNEL_LDFLAGS} ${SalomeIDLKernel} ${SalomeNS} ${SalomeContainer} ${SALOMELocalTrace} ${OMNIORB_LIBRARIES} SalomeIDLPARAVIS)
INSTALL(TARGETS PARAVISEngine DESTINATION lib/salome)
  
INSTALL(FILES PARAVIS_Engine_i.hh DESTINATION include/salome)
