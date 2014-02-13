# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

PROJECT( ParaMEDCorba )

  CMAKE_MINIMUM_REQUIRED( VERSION 2.8.7 )
  ## !!! This definition corrupts wrapping process
  #SET( LIBRARY_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin CACHE INTERNAL "Single place for output" )
  #SET( EXECUTABLE_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin  CACHE INTERNAL "Single place for output" )
  MARK_AS_ADVANCED(
    LIBRARY_OUTPUT_PATH
    EXECUTABLE_OUTPUT_PATH
  )

  FIND_PACKAGE( ParaView COMPONENTS pqCore NO_MODULE)

  IF( ParaView_FOUND )
    
    INCLUDE( ${PARAVIEW_USE_FILE} )

    FIND_PACKAGE( VTK 6.1 COMPONENTS vtkClientServer NO_MODULE)
    INCLUDE( ${VTK_USE_FILE} )

    SET( PARACORBAPLUG_SRCS
      ${ParaMEDCorba_SOURCE_DIR}/vtkParaMEDCorbaSource.cxx
      )

    ADD_LIBRARY (ParaMEDMEM2VTK SHARED VTKMEDCouplingMeshClient.cxx VTKMEDCouplingUMeshClient.cxx VTKMEDCouplingCMeshClient.cxx
      VTKMEDCouplingFieldClient.cxx VTKParaMEDFieldClient.cxx VTKMEDCouplingMultiFieldsClient.cxx VTKMEDCouplingCurveLinearMeshClient.cxx)
    
    ADD_PARAVIEW_PLUGIN( ParaMEDCorbaPlugin "1.0"
      SERVER_MANAGER_XML ParaMEDCorbaServerManager.xml
      GUI_RESOURCE_FILES
      ParaMEDCorbaUserInterfaceSources.xml
      SERVER_MANAGER_SOURCES ${PARACORBAPLUG_SRCS} )
    
    INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR} ${MED_ROOT_DIR}/include/salome ${KERNEL_ROOT_DIR}/include/salome)
    SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS}")
    SET_TARGET_PROPERTIES(ParaMEDCorbaPlugin PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS}")

    TARGET_LINK_LIBRARIES(ParaMEDMEM2VTK
      #vtkCommon
      #vtkFiltering
      #vtkGraphics
      vtkClientServer
      pqCore
      ${OMNIORB_LIBRARIES}
      ${MED_SalomeIDLMED}
      ${KERNEL_SalomeIDLKernel}
      ${QT_LIBRARIES}
      )
    TARGET_LINK_LIBRARIES(ParaMEDCorbaPlugin
      ParaMEDMEM2VTK
      )
    
    INSTALL(TARGETS ParaMEDCorbaPlugin DESTINATION lib/paraview)

    INSTALL(TARGETS ParaMEDMEM2VTK DESTINATION lib/salome)
    
    ADD_EXECUTABLE( testParaMEDCorba
      ${ParaMEDCorba_SOURCE_DIR}/testParaMEDCorba.cxx
      )
    # SET_TARGET_PROPERTIES(testCorbaPlugin PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS}")
    
    TARGET_LINK_LIBRARIES( testParaMEDCorba
      ParaMEDCorbaPlugin
      )
    
    INSTALL(FILES ParaMEDCorbaServerManager.xml DESTINATION share)
    
  
  ENDIF( ParaView_FOUND )

