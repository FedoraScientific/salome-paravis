#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# This Makefile is responsible of generating the client and server
# implementation of IDL interfaces for both C++ and python usage.
# The building process of the C++ files is in charge of each source
# package and then is not manage here.
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

#Executable
bin_PROGRAMS = vtkWrapIDL 

dist_vtkWrapIDL_SOURCES = ${PVHOME}/src/vtkParse.tab.c vtkWrapIDL.c

vtkWrapIDL_CPPFLAGS = \
	${PARAVIEW_INCLUDES}

include $(srcdir)/wrap.am

BASEIDL_FILES = PARAVIS_Gen_Types.idl PARAVIS_Gen.idl 
BASEIDL_SK_FILES= PARAVIS_Gen_TypesSK.cc PARAVIS_GenSK.cc

# This variable defines the files to be installed
dist_salomeidl_DATA = $(BASEIDL_FILES) $(WRAP_IDL)

# PARAVIS idl common library
lib_LTLIBRARIES = libSalomeIDLPARAVIS.la

# Sources built from idl files
nodist_libSalomeIDLPARAVIS_la_SOURCES = $(BASEIDL_SK_FILES) $(WRAP_SK_FILES)

# header files must be exported: other modules have to use this library
nodist_salomeinclude_HEADERS = $(BASEIDL_FILES:%.idl=%.hh) $(WRAP_IDL:%.idl=%.hh)

libSalomeIDLPARAVIS_la_CPPFLAGS =			\
	-I$(top_builddir)/salome_adm/unix	\
	-I$(top_builddir)/idl			\
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@ @KERNEL_CXXFLAGS@ 
libSalomeIDLPARAVIS_la_LDFLAGS = -no-undefined -version-info=0:0:0
libSalomeIDLPARAVIS_la_LIBADD  = @CORBA_LIBS@ 

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
OMNIORB_IDLPYFLAGS  = @OMNIORB_IDLPYFLAGS@ -I$(KERNEL_ROOT_DIR)/idl/salome 
IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome -I$(top_builddir)/salome_adm/unix
IDLPYFLAGS  = -bpythonIDL -p$(srcdir) -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome

# potential problem on parallel make on the following - multiple outputs
SUFFIXES = .idl .hh SK.cc
.idlSK.cc:
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<
.idl.hh:
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

install-exec-local: $(BASEIDL_FILES:%=$(top_srcdir)/idl/%) $(WRAP_IDL)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ; \
	done

# uninstall-local removes too much, but it works in distcheck
uninstall-local:
	rm -rf $(DESTDIR)$(salomepythondir)/*

mostlyclean-local: clean-idls

clean-idls:
	@find . \( -name "*.hh" -or -name "*.cc" -or -name ".depidl" -or -name "*.idl" \) -exec rm -f {} \;

# we use cpp to generate dependencies between idl files.
# option x c tells the preprocessor to consider idl as a c file.
# if an idl is modified, all idl dependencies are rebuilt

.depidl: $(BASEIDL_FILES)
	@echo "" > $@
	@for dep in $^ dummy; do \
	  if [ $$dep != "dummy" ]; then \
	    echo Building dependencies for $$dep; \
	    $(CPP) $(C_DEPEND_FLAG) $(C_WRAP_IDL_FLAG) -x c -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome $$dep 2>/dev/null | \
	    sed 's/\.o/\SK.cc/' >>$@; \
	  fi; \
	done ;

-include .depidl
