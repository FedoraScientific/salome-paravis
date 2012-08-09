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

IF(PARAVIEW_BUILD_QT_GUI)
   
    # Custom object panel for the reader and 
    # custom display panel for line chart view
    QT4_WRAP_UI(CLIENT_UI_SRCS
        Resources/UI/TableReaderPanel.ui
	Resources/UI/CustomXYChartDisplayPanel.ui
    )

    QT4_WRAP_CPP(MOC_SRCS 
	pqTableReaderPanel.h
	pqTableTo3DPanel.h
	pqCustomChartDisplayPanelImplementation.h
	pqCustomXYChartDisplayPanel.h
	pqCustomPlotSettingsModel.h
    )
        
     SET(CLIENT_SRCS
        pqTableReaderPanel.cxx
        pqTableTo3DPanel.cxx
	pqCustomChartDisplayPanelImplementation.cxx
	pqCustomXYChartDisplayPanel.cxx
	pqCustomPlotSettingsModel.cxx
    )	

    ADD_PARAVIEW_OBJECT_PANEL(
        TABLEREADER_IFACES TABLEREADER_IFACE_SRCS 
        CLASS_NAME pqTableReaderPanel
        XML_NAME TableReader 
	XML_GROUP sources)
    
    ADD_PARAVIEW_OBJECT_PANEL(
        TABLETO3D_IFACES TABLETO3D_IFACE_SRCS 
        CLASS_NAME pqTableTo3DPanel
        XML_NAME TableTo3D 
	XML_GROUP filters)

    # define the server-side and the client-side parts
    # of the table reader plugin
    ADD_PARAVIEW_PLUGIN(TableReaderPlugin "1.0"
        SOURCES ../Reader/TableParser.cxx
	SERVER_MANAGER_SOURCES ../Reader/vtkVisuTableReader.cxx ../TableTo3DFilter/vtkTableTo3D.cxx
	SERVER_MANAGER_XML Resources/TableReaderServer.xml Resources/TableTo3DServer.xml
	GUI_INTERFACES ${TABLEREADER_IFACES} ${TABLETO3D_IFACES} pqCustomChartDisplayPanel
	GUI_SOURCES ${CLIENT_SRCS} 
	    ${CLIENT_UI_SRCS} 
	    ${TABLEREADER_IFACE_SRCS} 
	    ${TABLETO3D_IFACE_SRCS}
	    ${MOC_SRCS}
	GUI_RESOURCES Resources/TableReader.qrc Resources/TableTo3D.qrc)
      
ELSE(PARAVIEW_BUILD_QT_GUI)

    # define the server-side part of the table reader plugin
    ADD_PARAVIEW_PLUGIN(TableReaderPlugin "1.0" 
      SOURCES TableParser.cxx
      SERVER_MANAGER_SOURCES vtkVisuTableReader.cxx vtkTableTo3D.cxx
      SERVER_MANAGER_XML Resources/TableReaderServer.xml Resources/TableTo3DServer.xml)

ENDIF(PARAVIEW_BUILD_QT_GUI)

TARGET_LINK_LIBRARIES(TableReaderPlugin vtkVisuTableReader vtkTableTo3D)

# Install
INSTALL(
    TARGETS TableReaderPlugin
    RUNTIME DESTINATION lib/paraview
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION lib/paraview
)