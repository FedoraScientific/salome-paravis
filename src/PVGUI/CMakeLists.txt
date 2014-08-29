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

INCLUDE(UseQt4Ext)

# --- options ---

INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR} 
    ${KERNEL_INCLUDE_DIRS}
    ${GUI_INCLUDE_DIRS} 
    ${PROJECT_BINARY_DIR}/idl 
    ${PROJECT_BINARY_DIR} 
    ${CAS_INCLUDE_DIRS} 
    ${Boost_INCLUDE_DIRS}
    ${QT_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS})
ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

# libraries to link to
SET(_PARAVIEW_APP_COMPO_LIB
  pqApplicationComponents
  )

SET(_link_LIBRARIES 
  ${OMNIORB_LIBRARIES}
  ${QT_LIBRARIES}
  ${CAS_KERNEL}
  ${PYTHON_LIBS}
  ${KERNEL_LDFLAGS} ${KERNEL_SalomeDS} ${KERNEL_SALOMELocalTrace} ${KERNEL_SalomeIDLKernel} 
                    ${KERNEL_SalomeNS} ${KERNEL_SalomeLifeCycleCORBA} ${KERNEL_SalomeContainer} ${KERNEL_TOOLSDS}
  ${GUI_LDFLAGS} ${GUI_SalomeApp} ${GUI_LightApp} ${GUI_CAM} ${GUI_qtx} ${GUI_suit} ${GUI_Event} ${GUI_PyInterp} 
                 ${GUI_SalomeObject} ${GUI_vtkTools}
  ${MED_SalomeIDLMED}
  ${_PARAVIEW_APP_COMPO_LIB}
  SalomeIDLPARAVIS
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PVGUI_Module.h
  PVGUI_ViewManager.h
  PVGUI_ViewModel.h
  PVGUI_ViewWindow.h
  PVGUI_ParaViewSettingsPane.h
)

# header files / no moc processing
SET(_other_HEADERS
  PVGUI_OutputWindowAdapter.h
  PVGUI_Tools.h
  #PVGUI_MatplotlibMathTextUtilities.h
  PV_I.h
  PARAVIS_Gen_i.hh
  PV_Events.h
)

IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
    LIST(APPEND _other_HEADERS PV_Tools.h)
ENDIF()

# header files / to install
SET(PVGUI_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/PARAVIS_images.ts
  resources/PARAVIS_msg_en.ts
  resources/PARAVIS_msg_fr.ts
  resources/PARAVIS_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  # sampleservers.pvsc ???
)

IF(SALOME_BUILD_DOC)
  # --- ParaView documentation is built as a resource ---
  # Check ParaView documentation file existence
  # if it exists add it to the resources (compile into binary form)
  # otherwise print warning message
  SET(PV_DOC_FILE ${VTK_INSTALL_PREFIX}/share/doc/paraview-${PARAVIEW_VERSION_MAJOR}.${PARAVIEW_VERSION_MINOR}/paraview.qch)
  IF (EXISTS ${PV_DOC_FILE})
    SET(RC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/PVGUI.qrc.in)
    SET(RC_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/PVGUI.qrc)
    MESSAGE(STATUS "Creation of ${RC_GENERATED}")
    CONFIGURE_FILE(${RC_SOURCE} ${RC_GENERATED})
    SET(_rcc_RESOURCES ${RC_GENERATED})
    ADD_DEFINITIONS(-DHAS_PV_DOC)
  ELSE ()
    MESSAGE(WARNING "Help file for ParaView has not been found! ParaView documentation will not be available.")
  ENDIF ()
ENDIF()

# --- sources ---

# sources / rcc wrappings
QT4_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / EDF specific stuff
INCLUDE(vtkWrapClientServer)
VTK_WRAP_ClientServer(vtkEDFCutterCS vtkEDFCutter_SRCS ${GUI_ROOT_DIR}/include/salome/vtkEDFCutter.h)

# sources / static
SET(_other_SOURCES
  PVGUI_Module.cxx
  PVGUI_Module_actions.cxx
  PVGUI_Module_widgets.cxx
  PVGUI_ViewManager.cxx
  PVGUI_ViewModel.cxx
  PVGUI_ViewWindow.cxx
  PVGUI_OutputWindowAdapter.cxx
  PVGUI_Tools.cxx
  PVGUI_ParaViewSettingsPane.cxx
  #PVGUI_MatplotlibMathTextUtilities.cxx
  #PV_Tools.cxx
  )

IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
  # Generate headers and sources corresponding to the IDL implementations:
  INCLUDE(${PROJECT_SOURCE_DIR}/src/VTKWrapping/wrap.cmake)

  ## From 2.8.10, we should use:
  ## SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  # Make the list of wrapped files visible from the top directory (will be used in idl):
  SET(WRAP_IDL_I_HH "${WRAP_IDL_I_HH}" PARENT_SCOPE)
  SET(WRAP_IDL_I_CC "${WRAP_IDL_I_CC}" PARENT_SCOPE)

  SET(_VTK_wrap_SOURCES
    PARAVIS_Gen_i.cc
    PARAVIS_CreateClass.cxx
    ${WRAP_IDL_I_CC}
  )
ENDIF()
  
# sources / to compile
SET(PVGUI_SOURCES 
  ${_other_SOURCES} 
  ${_moc_SOURCES} 
  ${_rcc_SOURCES} 
  ${vtkEDFCutter_SRCS} 
  ${_VTK_wrap_SOURCES}
  )

# --- rules ---

IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_CreateClass.cxx
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/VTKWrapping/create_class.py ${CMAKE_BINARY_DIR}
    DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt 
         ${PROJECT_BINARY_DIR}/idl/PARAVIS_Gen.hh 
         ${PROJECT_BINARY_DIR}/idl/PARAVIS_Gen_Types.hh
         SalomeIDLPARAVIS
         generate_implements
  )

  # All wrapping tools have to be build before generating the above module - NEEDED?
  ###ADD_DEPENDENCIES(SalomeIDLPARAVIS vtkWrapIDL_HH vtkWrapIDL_CC vtkWrapIDL)

  # Intermediate target used in PVGUI directory (we can not cross reference
  # a generated source file across directories, hence this trick):
  INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/idl
    ${PROJECT_BINARY_DIR}/src/VTKWrapping
  )
ENDIF()

ADD_LIBRARY(PARAVIS ${PVGUI_SOURCES})
IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
  # Make sure the generation of the VTK wrapping get executed first:
  ADD_DEPENDENCIES(PARAVIS PARAVIS_CreateClass.cxx)
ENDIF()

IF(NOT SALOME_PARAVIS_MINIMAL_CORBA)
  # Ensure .hh files corresponding to IDL have been generated first:
  ADD_DEPENDENCIES(PARAVIS omniorb_module_SalomeIDLPARAVIS)  
ENDIF()

TARGET_LINK_LIBRARIES(PARAVIS ${_link_LIBRARIES})
INSTALL(TARGETS PARAVIS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})


# --- header and resources installation ---
INSTALL(FILES ${PVGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_PARAVIS_INSTALL_RES_DATA}")
INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_PARAVIS_INSTALL_RES_DATA})
