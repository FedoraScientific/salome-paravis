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

# create a plugin with a custom view that shows up in ParaView's multi-view
# manager.  this plugin also contains a custom display panel

# moc the Qt based .h files
QT4_WRAP_CPP(MOC_SRCS MyView.h MyDisplay.h MyViewActiveOptions.h MyViewOptions.h)

# invoke macro to create sources for our custom view and display panel
ADD_PARAVIEW_VIEW_MODULE(
                         # returns the interfaces defined (pass in
                         # GUI_INTERFACES parameter)
                         IFACES  
                         # returns a list of source files for this interface
                         IFACE_SRCS 
                         # give the view type 
                         # With MyView.h implementing a
                         # pqGenericViewModule and MyView being the XML name
                         # for the view on the server side
                         VIEW_TYPE MyView 
                         # the XML group of the view in the server manager xml
                         VIEW_XML_GROUP views
                         # the XML name of the display for this view
                         DISPLAY_XML MyDisplay 
                         # the name of the display panel for this display
                         # With MyDisplay.h implementing pqDisplayPanel
                         DISPLAY_PANEL MyDisplay)


ADD_PARAVIEW_VIEW_OPTIONS(OPTIONS_IFACE OPTIONS_IFACE_SRCS
                          VIEW_TYPE MyView ACTIVE_VIEW_OPTIONS MyViewActiveOptions)

# create a GUI side plugin with the GUI side code
#ADD_PARAVIEW_PLUGIN(GUISampleView "1.0" GUI_INTERFACES ${IFACES} ${OPTIONS_IFACE}
#                    GUI_SOURCES MyView.cxx MyDisplay.cxx MyViewActiveOptions.cxx MyViewOptions.cxx
#                    ${MOC_SRCS} ${IFACE_SRCS} ${OPTIONS_IFACE_SRCS})

# create a server side plugin with the server side code
#ADD_PARAVIEW_PLUGIN(SMSampleView "1.0" SERVER_MANAGER_XML MyViewSM.xml)

  ADD_PARAVIEW_PLUGIN(GUISampleView "1.0"
    SERVER_MANAGER_XML MyViewSM.xml
    GUI_INTERFACES ${IFACES} ${OPTIONS_IFACE}
    GUI_SOURCES MyView.cxx MyDisplay.cxx MyViewActiveOptions.cxx MyViewOptions.cxx
    ${MOC_SRCS} ${IFACE_SRCS} ${OPTIONS_IFACE_SRCS} )
# one could combine the two plugins into one if desired

INSTALL(
	TARGETS GUISampleView
	DESTINATION lib/paraview
)
