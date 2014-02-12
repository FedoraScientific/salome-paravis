# Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

INCLUDE_DIRECTORIES(
  ${MEDReader_SOURCE_DIR}/IO
  ${MEDReader_BINARY_DIR}/IO )

IF(PARAVIEW_BUILD_QT_GUI)

  QT4_WRAP_UI(CLIENT_UI_SRCS Resources/UI/MEDReaderPanel.ui Resources/UI/ExtractGroupPanel.ui Resources/UI/ExtractCellTypePanel.ui)
  
  # create a plugin that implements an object panel for the reader
  SET(CLIENT_SRCS pqMEDReaderPanel.cxx pqExtractGroupPanel.cxx pqExtractCellTypePanel.cxx VectBoolSpreadSheet.cxx VectBoolWidget.cxx)
  
  QT4_WRAP_CPP(MOC_SRCS pqMEDReaderPanel.h pqExtractGroupPanel.h pqExtractCellTypePanel.h VectBoolSpreadSheet.h VectBoolWidget.h)
  
  ADD_PARAVIEW_OBJECT_PANEL(MEDREADER_IFACES MEDREADER_IFACE_SRCS
    CLASS_NAME pqMEDReaderPanel
    XML_NAME MEDReader
    XML_GROUP sources
    )
  
  ADD_PARAVIEW_OBJECT_PANEL(EXTRACTGROUP_IFACES EXTRACTGROUP_IFACE_SRCS
    CLASS_NAME pqExtractGroupPanel
    XML_NAME ExtractGroup
    XML_GROUP filters
    )

  ADD_PARAVIEW_OBJECT_PANEL(EXTRACTCELLTYPE_IFACES EXTRACTCELLTYPE_IFACE_SRCS
    CLASS_NAME pqExtractCellTypePanel
    XML_NAME ExtractCellType
    XML_GROUP filters
    )
  
  # create the med reader plugin itself
  ADD_PARAVIEW_PLUGIN(MEDReaderPlugin "4.0"
    SERVER_MANAGER_XML Resources/MEDReaderServer.xml
    GUI_INTERFACES ${MEDREADER_IFACES} ${EXTRACTGROUP_IFACES} ${EXTRACTCELLTYPE_IFACES}
    GUI_RESOURCES Resources/pqMEDReader.qrc
    GUI_SOURCES ${CLIENT_SRCS}
    ${CLIENT_UI_SRCS}
    ${MOC_SRCS}
    ${MEDREADER_IFACE_SRCS} ${EXTRACTGROUP_IFACE_SRCS} ${EXTRACTCELLTYPE_IFACE_SRCS}
    CS_KITS
    # this is the same name as the name in module.cmake file IO
    # directory.
    vtkMEDReader
    )
  
ELSE(PARAVIEW_BUILD_QT_GUI)
  
  # create the med reader plugin itself
  ADD_PARAVIEW_PLUGIN(MEDReaderPlugin "4.0"
    SERVER_MANAGER_SOURCES ${SM_SRCS}
    SERVER_MANAGER_XML Resources/MEDReaderServer.xml
    CS_KITS
    # this is the same name as the name in module.cmake file IO
    # directory.
    vtkMEDReader
    )
  
ENDIF(PARAVIEW_BUILD_QT_GUI)

TARGET_LINK_LIBRARIES(MEDReaderPlugin vtkMEDReader vtkMEDReaderCS)
# Install
INSTALL(
  TARGETS MEDReaderPlugin
  RUNTIME DESTINATION lib/paraview
  LIBRARY DESTINATION lib/paraview
  ARCHIVE DESTINATION lib/paraview
  )




