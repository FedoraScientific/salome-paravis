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

#Executable
bin_PROGRAMS = vtkWrapIDL_HH vtkWrapIDL_CC

dist_vtkWrapIDL_HH_SOURCES = ${PVHOME}/src/vtkParse.tab.c $(top_srcdir)/idl/vtkWrapIDL.c
dist_vtkWrapIDL_CC_SOURCES = ${PVHOME}/src/vtkParse.tab.c $(top_srcdir)/idl/vtkWrapIDL.c

vtkWrapIDL_HH_CPPFLAGS = -DIDL_I_HH	${PARAVIEW_INCLUDES}


vtkWrapIDL_CC_CPPFLAGS = -DIDL_I_CC	${PARAVIEW_INCLUDES}

include $(srcdir)/wrap.am

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
	PVGUI_ViewWindow.cxx \
	PVGUI_OutputWindowAdapter.h \
	PVGUI_OutputWindowAdapter.cxx \
	PVGUI_Tools.h \
	PVGUI_Tools.cxx \
	PVGUI_Trace.h \
	PVGUI_Trace.cxx \
	PV_I.h \
	PARAVIS_Gen_i.hh \
	PV_Events.h \
	PV_Tools.h \
	PARAVIS_Gen_i.cc \
	PV_Tools.cxx \
	$(WRAP_IDL_I_HH) \
	$(WRAP_IDL_I_CC) \
	PARAVIS_CreateClass.cxx

# MOC pre-processing
MOC_FILES = \
	PVGUI_Module_moc.cxx \
	PVGUI_ViewManager_moc.cxx \
	PVGUI_ViewModel_moc.cxx \
	PVGUI_ViewWindow_moc.cxx \
	PVGUI_Trace_moc.cxx

nodist_libPARAVIS_la_SOURCES = $(MOC_FILES)

# additionnal compilation flags
# Note that ParaView uses HDF5
libPARAVIS_la_CPPFLAGS = \
	-ftemplate-depth-32 \
	$(HDF5_INCLUDES) \
	$(QT_INCLUDES) \
	$(QT_ASSISTANT_INCLUDES) \
	$(PARAVIEW_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(PYTHON_INCLUDES) \
	@CAS_CPPFLAGS@ @CAS_CXXFLAGS@ \
	$(KERNEL_CXXFLAGS) \
	$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) \
	$(GUI_CXXFLAGS) \
	$(VISU_CXXFLAGS) \
	$(MED_CXXFLAGS) \
	-I$(top_builddir)/idl \
	-I$(srcdir)/../../idl	\
	-I$(top_builddir)/salome_adm/unix

# additionnal linkage flags
# Note that ParaView uses HDF5
libPARAVIS_la_LDFLAGS = \
	$(PYTHON_LIBS) \
	$(QT_MT_LIBS) \
	$(HDF5_LIBS) \
	$(QT_ASSISTANT_LIBS) \
	$(PARAVIEW_LIBS) \
	../../idl/libSalomeIDLPARAVIS.la \
	$(KERNEL_LDFLAGS) -lSalomeNS -lSalomeDSClient -lSalomeContainer -lSalomeLifeCycleCORBA \
	$(GUI_LDFLAGS) -lSalomeApp -lLightApp \
	$(VISU_LDFLAGS) -lSalomeIDLVISU

libPARAVIS_la_LIBADD= \
	-lSalomeIDLKernel \
	-lSalomeHDFPersist \
	-lSalomeGenericObj \
	-lSalomeContainer \
	-lSalomeCommunication \
	-lSalomeNS \
	-lTOOLSDS \
	-lOpUtil \
	-lSalomeLifeCycleCORBA \
	-lEvent \
	-lSalomeApp \
	-lSalomeSession \
	-lqtx \
	-lLightApp

#	
# resources files
nodist_salomeres_DATA = \
           PARAVIS_images.qm \
           PARAVIS_msg_en.qm

mostlyclean-local: clean-wrap

clean-wrap:
	-rm -f *.hh *.cc
