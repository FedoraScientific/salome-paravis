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

# add include indirectories to the vtk files in this plugin.
include_directories(
  ${MedReader_SOURCE_DIR}/IO
  ${MedReader_BINARY_DIR}/IO)


SET(SM_SRCS
    vtkSMMedGroupSelectionDomain.cxx
    )

IF(PARAVIEW_BUILD_QT_GUI)

    QT4_WRAP_UI(CLIENT_UI_SRCS
        Resources/UI/MedReaderPanel.ui
        Resources/UI/ExtractGroupPanel.ui
    )

    # create a plugin that implements an object panel for the reader
    SET(CLIENT_SRCS
        pqMedReaderPanel.cxx
        pqExtractGroupPanel.cxx
    )

    QT4_WRAP_CPP(MOC_SRCS
        pqMedReaderPanel.h
        pqExtractGroupPanel.h
    )

    ADD_PARAVIEW_OBJECT_PANEL(MEDREADER_IFACES MEDREADER_IFACE_SRCS
        CLASS_NAME pqMedReaderPanel
        XML_NAME MedReader
        XML_GROUP sources
    )
        
    ADD_PARAVIEW_OBJECT_PANEL(EXTRACTGROUP_IFACES EXTRACTGROUP_IFACE_SRCS
        CLASS_NAME pqExtractGroupPanel
        XML_NAME ExtractGroup
        XML_GROUP filters
    )
    
    # create the med reader plugin itself
    ADD_PARAVIEW_PLUGIN(MedReaderPlugin "4.0"
        SERVER_MANAGER_SOURCES ${SM_SRCS}
        SERVER_MANAGER_XML Resources/MedReaderServer.xml
        GUI_INTERFACES ${MEDREADER_IFACES} ${EXTRACTGROUP_IFACES}
        GUI_RESOURCES Resources/pqMedReader.qrc
        GUI_SOURCES ${CLIENT_SRCS}
              ${CLIENT_UI_SRCS}
              ${MOC_SRCS}
              ${MEDREADER_IFACE_SRCS}
              ${EXTRACTGROUP_IFACE_SRCS}
        CS_KITS
            # this is the same name as the name in module.cmake file IO
            # directory.
            vtkMedReader
    )
        
ELSE(PARAVIEW_BUILD_QT_GUI)
          
    # create the med reader plugin itself
    ADD_PARAVIEW_PLUGIN(MedReaderPlugin "4.0"
        SERVER_MANAGER_SOURCES ${SM_SRCS}
        SERVER_MANAGER_XML Resources/MedReaderServer.xml
        CS_KITS
            # this is the same name as the name in module.cmake file IO
            # directory.
            vtkMedReader
    )
    
ENDIF(PARAVIEW_BUILD_QT_GUI)

TARGET_LINK_LIBRARIES(MedReaderPlugin vtkMedReader vtkMedReaderCS)

# Install
INSTALL(
    TARGETS MedReaderPlugin
    RUNTIME DESTINATION lib/paraview
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION lib/paraview
)




