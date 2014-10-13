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

IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
    INCLUDE_DIRECTORIES(
      ${OMNIORB_INCLUDE_DIR} 
      ${KERNEL_INCLUDE_DIRS} 
      ${PROJECT_BINARY_DIR}/idl)
    
    ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
    ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})
    
    ADD_LIBRARY(PARAVISEngine PARAVIS_Engine_i.cc)
    
    TARGET_LINK_LIBRARIES(PARAVISEngine 
      ${KERNEL_SalomeIDLKernel} ${KERNEL_SalomeNS} ${KERNEL_SalomeContainer} ${KERNEL_SALOMELocalTrace} 
      ${OMNIORB_LIBRARIES} 
      SalomeIDLPARAVIS)
    INSTALL(TARGETS PARAVISEngine)  # doesn't need to be exported
      
    INSTALL(FILES PARAVIS_Engine_i.hh DESTINATION ${SALOME_INSTALL_HEADERS})
ELSE()
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/no_wrap/PARAVIS.py.in ${CMAKE_CURRENT_BINARY_DIR}/PARAVIS.py @ONLY)
    SET(_bin_SCRIPTS
        ${CMAKE_CURRENT_BINARY_DIR}/PARAVIS.py
        no_wrap/PARAVIS_utils.py
    )

    # --- rules ---
    SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})
ENDIF()
    