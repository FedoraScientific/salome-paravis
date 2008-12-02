#  PARAVIS : ParaView wrapper SALOME module
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  Author : Julia DOROVSKIKH
#  Date   : 01/06/2005
#  Modified by : Alexander BORODIN (OCN) - autotools usage

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Library target
lib_LTLIBRARIES = libPARAVIS.la

# Library sources
dist_libPARAVIS_la_SOURCES =	\
	PVGUI_Module.h		\
	PVGUI_Module_impl.h     \
	PVGUI_Module.cxx        \
	PVGUI_Module_actions.cxx \
	PVGUI_Module_widgets.cxx \
	PVGUI_ProcessModuleHelper.h \
	PVGUI_ProcessModuleHelper.cxx \
	PVGUI_ViewManager.h \
	PVGUI_ViewManager.cxx \
	PVGUI_ViewModel.h \
	PVGUI_ViewModel.cxx \
	PVGUI_ViewWindow.h \
	PVGUI_ViewWindow.cxx

# MOC pre-processing
MOC_FILES = \
	PVGUI_Module_moc.cxx \
	PVGUI_ViewManager_moc.cxx \
	PVGUI_ViewModel_moc.cxx \
	PVGUI_ViewWindow_moc.cxx

nodist_libPARAVIS_la_SOURCES = $(MOC_FILES)

# additionnal compilation flags
libPARAVIS_la_CPPFLAGS = \
	$(QT_INCLUDES) \
	$(QT_ASSISTANT_INCLUDES) \
	$(PARAVIEW_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(PYTHON_INCLUDES) \
	$(GUI_CXXFLAGS)

# additionnal linkage flags
libPARAVIS_la_LDFLAGS = \
	$(QT_ASSISTANT_LIBS) \
	$(PARAVIEW_LIBS) \
	$(GUI_LDFLAGS) \
	-lLightApp 

# resources files
nodist_salomeres_DATA = \
           PARAVIS_images.qm \
           PARAVIS_msg_en.qm
