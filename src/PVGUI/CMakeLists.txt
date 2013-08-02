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

SET(VERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MAINTENANCE})
SET(SOVERSION_INFO ${VERSION_MAJOR}.${VERSION_MINOR})

ADD_DEFINITIONS(-DHAVE_SALOME_CONFIG ${CAS_DEFINITIONS})

INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR} 
    ${KERNEL_ROOT_DIR}/include/salome
    ${GUI_ROOT_DIR}/include/salome 
    ${CMAKE_CURRENT_SOURCE_DIR}/../ParaView 
    ${PROJECT_BINARY_DIR}/idl 
    ${PROJECT_BINARY_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CAS_INCLUDE_DIRS} 
    ${Boost_INCLUDE_DIRS}
    ${QT_INCLUDES}
    )

INCLUDE(${QT_USE_FILE})

#IF(WITH_VISU)
#  SET(VISU_PARAVIS_LIBS
#    ${SalomeIDLVISU}
#    )
#  
#  ADD_DEFINITIONS(-DWITH_VISU)
#ENDIF(WITH_VISU)

#Executable
SET(bin_PROGRAMS
  vtkWrapIDL_HH
  vtkWrapIDL_CC
  )

SET(vtkWrapIDL_HH_CPPFLAGS
  -DIDL_I_HH
  )

SET(vtkWrapIDL_CC_CPPFLAGS
  -DIDL_I_CC
  )
#VTN TODO: vtkParseData supports two types of structures for wrapping.
#We use old structures currently, but is is not influent for results of wrapping.
#It may be necessary to port vtkWrapIDL to new approach in future.
#IF(VTK_LEGACY_REMOVE)
#  ADD_DEFINITIONS(-DVTK_PARSE_LEGACY_REMOVE)
#ENDIF()

INCLUDE(wrap.cmake)

INCLUDE(${ParaView_CMAKE_DIR}/vtkWrapClientServer.cmake)
VTK_WRAP_ClientServer(vtkEDFCutterCS vtkEDFCutter_SRCS ${GUI_ROOT_DIR}/include/salome/vtkEDFCutter.h)

# Library sources
SET(dist_libPARAVIS_la_SOURCES
  PVGUI_Module.h
  PVGUI_Module.cxx
  PVGUI_Module_actions.cxx
  PVGUI_Module_widgets.cxx
  PVGUI_ViewManager.h
  PVGUI_ViewManager.cxx
  PVGUI_ViewModel.h
  PVGUI_ViewModel.cxx
  PVGUI_ViewWindow.h
  PVGUI_ViewWindow.cxx
  PVGUI_OutputWindowAdapter.h
  PVGUI_OutputWindowAdapter.cxx
  PVGUI_Tools.h
  PVGUI_Tools.cxx
  PVGUI_ParaViewSettingsPane.h
  PVGUI_ParaViewSettingsPane.cxx
  PVGUI_MatplotlibMathTextUtilities.h
  PVGUI_MatplotlibMathTextUtilities.cxx
  PV_I.h
  PARAVIS_Gen_i.hh
  PV_Events.h
  PV_Tools.h
  PARAVIS_Gen_i.cc
  PV_Tools.cxx
  ${WRAP_IDL_I_HH}
  ${WRAP_IDL_I_CC}
  PARAVIS_CreateClass.cxx
  ${vtkEDFCutter_SRCS}
  )

# Check ParaView documentation file existence
# if it exists add it to the resources (compile into binary form)
# otherwise print warning message
SET(PV_DOC_FILE ${VTK_INSTALL_PREFIX}/share/doc/paraview-${PARAVIEW_VERSION_MAJOR}.${PARAVIEW_VERSION_MINOR}/paraview.qch)
IF (EXISTS ${PV_DOC_FILE})
  SET(RC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/PVGUI.qrc.in)
  SET(RC_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/PVGUI.qrc)
  MESSAGE(STATUS "Creation of ${RC_GENERATED}")
  CONFIGURE_FILE(${RC_SOURCE} ${RC_GENERATED})
  QT4_ADD_RESOURCES(RCS_SOURCES ${RC_GENERATED})
  SET(dist_libPARAVIS_la_SOURCES ${dist_libPARAVIS_la_SOURCES} ${RCS_SOURCES})
  ADD_DEFINITIONS(-DHAS_PV_DOC)
ELSE (EXISTS ${PV_DOC_FILE})
  MESSAGE(WARNING "Help file for ParaView has not been found! ParaView documentation will not be available.")
ENDIF (EXISTS ${PV_DOC_FILE})

# MOC pre-processing
SET(MOC_FILES
  PVGUI_Module_moc.cxx
  PVGUI_ViewManager_moc.cxx
  PVGUI_ViewModel_moc.cxx
  PVGUI_ViewWindow_moc.cxx
  PVGUI_ParaViewSettingsPane_moc.cxx
  )

# additionnal compilation flags
# Note that ParaView uses HDF5
SET(libPARAVIS_la_CPPFLAGS
  -ftemplate-depth-32
  ${QT_ASSISTANT_INCLUDES}
  ${PYTHON_INCLUDES}
  ${CAS_CPPFLAGS}
  ${CAS_CXXFLAGS}
  ${KERNEL_CXXFLAGS}
  ${OMNIORB_DEFINITIONS}
  ${BOOST_CPPFLAGS}
  ${GUI_CXXFLAGS}
  ${MED_CXXFLAGS}
  ${VISU_CXXFLAGS}
  ${VISU_PARAVIS_DEFINES}
  )

# remove deprecation warnings from Paraview
IF(WINDOWS)
ELSE(WINDOWS)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
ENDIF(WINDOWS)

# resources files
SET(nodist_salomeres_DATA
  PARAVIS_images.qm
  PARAVIS_msg_en.qm
  PARAVIS_msg_fr.qm
  )

SET(srcs)
FOREACH(src ${dist_libPARAVIS_la_SOURCES})
  SET(srcs ${srcs} ${src})
ENDFOREACH(src ${dist_libPARAVIS_la_SOURCES})

FOREACH(f ${MOC_FILES})
  SET(srcs ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/${f})
ENDFOREACH(f ${MOC_FILES})

ADD_LIBRARY(PARAVIS SHARED ${srcs})

SET(flags)
FOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${libPARAVIS_la_CPPFLAGS})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${libPARAVIS_la_CPPFLAGS})

SET_TARGET_PROPERTIES(PARAVIS PROPERTIES COMPILE_FLAGS "${flags}")
SET_TARGET_PROPERTIES(PARAVIS PROPERTIES VERSION ${VERSION_INFO} SOVERSION ${SOVERSION_INFO})

#FIND_PACKAGE(VTK 6.0 COMPONENTS vtkPVClientServerCoreCore vtkPVServerImplementationCore NO_MODULE)
#SET(VTK_LIBS vtkPVClientServerCoreCore vtkPVServerImplementationCore)
SET(VTK_LIBS)
FIND_PACKAGE(ParaView COMPONENTS pqApplicationComponents NO_MODULE)
SET(PARAVIEW_LIBS pqApplicationComponents)

TARGET_LINK_LIBRARIES(PARAVIS 
  ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS}
  ${OMNIORB_LIBRARIES}
  ${QT_LIBS}
  ${CAS_KERNEL}
  ${VTK_LIBS}
  ${PYTHON_LIBS}
  ${KERNEL_LDFLAGS} ${KERNEL_SalomeDS} ${KERNEL_SALOMELocalTrace} ${KERNEL_SalomeIDLKernel} 
                    ${KERNEL_SalomeNS} ${KERNEL_SalomeLifeCycleCORBA} ${KERNEL_SalomeContainer} ${KERNEL_TOOLSDS}
  ${GUI_LDFLAGS} ${GUI_SalomeApp} ${GUI_LightApp} ${GUI_CAM} ${GUI_qtx} ${GUI_suit} ${GUI_Event} ${GUI_PyInterp} 
                 ${GUI_SalomeObject} ${GUI_vtkTools}
  ${MED_LDFLAGS} ${MED_SalomeIDLMED}
  ${VISU_LDFLAGS}
  ${VISU_PARAVIS_LIBS}
  ${PARAVIEW_LIBS}
  SalomeIDLPARAVIS
)
INSTALL(TARGETS PARAVIS DESTINATION lib/salome)

FOREACH(name ${bin_PROGRAMS})
  ADD_EXECUTABLE(${name} 
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParse.tab.c 
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParsePreprocess.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseHierarchy.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseExtras.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseMain.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseData.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseString.c
    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkWrap.c
#    ${PROJECT_SOURCE_DIR}/src/ParaView/vtkWrapText.c
    ${PROJECT_SOURCE_DIR}/idl/vtkWrapIDL.c)
  ADD_DEPENDENCIES(${name} generate_txt)
  
  set_property(SOURCE ${PROJECT_SOURCE_DIR}/src/ParaView/vtkParseMain.c PROPERTY COMPILE_DEFINITIONS
    VTK_PARSE_VERSION="${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")

  SET(flags)
  FOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${name}_CPPFLAGS})
    SET(flags "${flags} ${f}")
  ENDFOREACH(f ${PARAVIEW_INCLUDES} ${VTK_INCLUDES} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${name}_CPPFLAGS})
  
  SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
  TARGET_LINK_LIBRARIES(${name} ${PLATFORM_LIBADD})
  
  SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${name})
  IF(WINDOWS)
    IF(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe)
    ELSE(CMAKE_BUILD_TOOL STREQUAL nmake)
      SET(${name}_EXEFILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${name}.exe)
    ENDIF(CMAKE_BUILD_TOOL STREQUAL nmake)
  ENDIF(WINDOWS)
  INSTALL(FILES ${${name}_EXEFILE} DESTINATION bin/salome PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDFOREACH(name ${bin_PROGRAMS})

FOREACH(output ${MOC_FILES})
  STRING(REGEX REPLACE _moc.cxx .h input ${output})
  
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${output})
  ADD_CUSTOM_COMMAND(
    OUTPUT ${output}
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_FLAGS} ${input} -o ${output}
    MAIN_DEPENDENCY ${input}
    )
ENDFOREACH(output ${MOC_FILES})
            
FOREACH(f ${nodist_salomeres_DATA})
  STRING(REGEX REPLACE .qm .ts input ${f})
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/resources/${input})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${f})
  EXECUTE_PROCESS(COMMAND ${QT_LRELEASE_EXECUTABLE} ${input} -qm ${output})
  INSTALL(FILES ${output} DESTINATION share/salome/resources/paravis)
ENDFOREACH(f ${nodist_salomeres_DATA})
