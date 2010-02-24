
        SET(VERSION_INFO 0.0.0)
        SET(SOVERSION_INFO 0)
        SET(SUBDIRS)
        SET(AM_CPPFLAGS)
        SET(AM_CXXFLAGS)
        SET(LDADD)
        

                SET(AM_CPPFLAGS ${AM_CPPFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                SET(AM_CXXFLAGS ${AM_CXXFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                
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
# include $(top_srcdir)/adm_local/unix/make_common_starter.am

#Executable
SET(bin_PROGRAMS
    vtkWrapIDL
)

# vtkWrapIDL.h:
# 	@$(srcdir)/create_header.sh $(top_srcdir)

# $(srcdir)/vtkWrapIDL.c: vtkWrapIDL.h;

SET(dist_vtkWrapIDL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParse.tab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vtkWrapIDL.c
)

SET(vtkWrapIDL_CPPFLAGS
    -I${CMAKE_SOURCE_DIR}/src/ParaView
    -I${CMAKE_BINARY_DIR}/idl
    ${PARAVIEW_INCLUDES}
    ${VTK_INCLUDES}
)

# hints: $(PVHOME)/lib/paraview-$(PVVERSION)/hints $(srcdir)/hints_paravis
# 	@$(srcdir)/create_hints.sh $(PVHOME)/lib/paraview-$(PVVERSION)

# $(srcdir)/wrap.am: $(top_srcdir)/wrapfiles.txt
# 	@cd $(srcdir) && $(srcdir)/create_wrap.sh

# include $(srcdir)/wrap.am
INCLUDE(wrap.cmake)

SET(BASEIDL_FILES
    PARAVIS_Gen_Types.idl
    PARAVIS_Gen.idl
)
SET(BASEIDL_SK_FILES
    PARAVIS_Gen_TypesSK.cc
    PARAVIS_GenSK.cc
)

SET(IDL_FILES
    ${BASEIDL_FILES}
    ${WRAP_IDL}
)

# This variable defines the files to be installed
SET(dist_salomeidl_DATA
    ${IDL_FILES}
)

# PARAVIS idl common library
SET(lib_LTLIBRARIES
    libSalomeIDLPARAVIS.la
)

# Sources built from idl files
SET(nodist_libSalomeIDLPARAVIS_la_SOURCES
    ${BASEIDL_SK_FILES}
    ${WRAP_SK_FILES}
)

# header files must be exported: other modules have to use this library
SET(nodist_salomeinclude_HEADERS
#    ${BASEIDL_FILES:%.idl=%.hh}
#    ${WRAP_IDL:%.idl=%.hh}
)

SET(libSalomeIDLPARAVIS_la_CPPFLAGS
    -I${CMAKE_BINARY_DIR}/salome_adm/unix
    -I${CMAKE_BINARY_DIR}/idl
    ${CORBA_CXXFLAGS}
    ${CORBA_INCLUDES}
    ${KERNEL_CXXFLAGS}
)
SET(VERSION_INFO 0.0.0)
SET(SOVERSION_INFO 0)
SET(libSalomeIDLPARAVIS_la_LDFLAGS
    -no-undefined
)
SET(libSalomeIDLPARAVIS_la_LIBADD
    ${CORBA_LIBS}
)

# These variables defines the building process of CORBA files
SET(OMNIORB_IDL
    ${OMNIORB_IDL}
)
SET(OMNIORB_IDLCXXFLAGS
    ${OMNIORB_IDLCXXFLAGS}
)
SET(OMNIORB_IDLPYFLAGS
    ${OMNIORB_IDLPYFLAGS}
    -I${KERNEL_ROOT_DIR}/idl/salome
)
SET(IDLCXXFLAGS
    -bcxx
    ${IDLCXXFLAGS}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -I${KERNEL_ROOT_DIR}/idl/salome
    -I${CMAKE_BINARY_DIR}/salome_adm/unix
)
SET(IDLPYFLAGS
    -bpythonIDL
    -nf
    -p${CMAKE_CURRENT_SOURCE_DIR}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -I${KERNEL_ROOT_DIR}/idl/salome
)

# potential problem on parallel make on the following - multiple outputs
SET(SUFFIXES
    .idl
    .hh
    SK.cc
)
# .idlSK.cc:
# 	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<
# .idl.hh:
# 	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

# install-exec-local: $(BASEIDL_FILES:%=$(top_srcdir)/idl/%) $(WRAP_IDL)
# 	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
# 	ls $^ | while read file; do  	  $(OMNIORB_IDL) $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ;  	done

# uninstall-local removes too much, but it works in distcheck
# uninstall-local:
# 	rm -rf $(DESTDIR)$(salomepythondir)/*

# mostlyclean-local: clean-idls

# clean-idls:
# 	@find . \( -name "*.hh" -or -name "*.cc" -or -name ".depidl" -or -name "*.idl" \) -exec rm -f {} \;
# 	-rm -f hints vtkWrapIDL.h

# we use cpp to generate dependencies between idl files.
# option x c tells the preprocessor to consider idl as a c file.
# if an idl is modified, all idl dependencies are rebuilt

# .depidl: $(BASEIDL_FILES)
# 	@echo "" > $@
# 	@for dep in $^ dummy; do  	  if [ $$dep != "dummy" ]; then  	    echo Building dependencies for $$dep;  	    $(CPP) $(C_DEPEND_FLAG) $(C_WRAP_IDL_FLAG) -x c -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome $$dep 2>/dev/null |  	    sed 's/\.o/\SK.cc/' >>$@;  	  fi;  	done ;

# -include .depidl


            FOREACH(dir ${SUBDIRS})
            IF(NOT dir STREQUAL .)
            ADD_SUBDIRECTORY(${dir})
            ENDIF(NOT dir STREQUAL .)
            ENDFOREACH(dir ${SUBDIRS})
            

        FOREACH(amname ${lib_LTLIBRARIES})
        

        STRING(REPLACE .la _la amname ${amname})
        

        STRING(LENGTH ${amname} len)
        MATH(EXPR newlen "${len}-3")
        STRING(SUBSTRING ${amname} 0 ${newlen} name)
        

        STRING(REGEX MATCH "^lib" BEGIN_WITH_lib ${name})
        IF(BEGIN_WITH_lib)
        STRING(LENGTH ${name} len)
        MATH(EXPR newlen "${len}-3")
        STRING(SUBSTRING ${name} 3 ${newlen} name)
        ENDIF(BEGIN_WITH_lib)
        

        STRING(REGEX MATCH "IDL" ISIDL ${name})
        

        IF(ISIDL)
        IF(WINDOWS)
        SET(type STATIC)
        ELSE(WINDOWS)
        SET(type SHARED)
        ENDIF(WINDOWS)
        ELSE(ISIDL)
        SET(type SHARED)
        ENDIF(ISIDL)
        

        SET(srcs)
        FOREACH(src ${${amname}_SOURCES} ${dist_${amname}_SOURCES})
        GET_FILENAME_COMPONENT(ext ${src} EXT)
        IF(ext STREQUAL .f)
        IF(src STREQUAL trte.f)
        SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${src})
        STRING(REPLACE ".f" ".c" src ${src})
        SET(src ${CMAKE_CURRENT_BINARY_DIR}/${src})
        SET(output ${src})
        ADD_CUSTOM_COMMAND(
        OUTPUT ${output}
        COMMAND f2c ${input}
        MAIN_DEPENDENCY ${input}
        )
        ELSE(src STREQUAL trte.f)
        SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${src})
        STRING(REPLACE ".f" ".o" src ${src})
        SET(src ${CMAKE_CURRENT_BINARY_DIR}/${src})
        SET(output ${src})
        IF(WINDOWS)
        SET(F77 g77)
        ELSE(WINDOWS)
        SET(F77 gfortran)
        ENDIF(WINDOWS)
        ADD_CUSTOM_COMMAND(
        OUTPUT ${output}
        COMMAND ${F77} -c -o ${output} ${input}
        MAIN_DEPENDENCY ${input}
        )
        ENDIF(src STREQUAL trte.f)
        ENDIF(ext STREQUAL .f)
        SET(srcs ${srcs} ${src})
        ENDFOREACH(src ${${amname}_SOURCES} ${dist_${amname}_SOURCES})
        

        SET(l ${nodist_${amname}_SOURCES} ${UIC_FILES})
        FOREACH(f ${l})
        SET(src ${CMAKE_CURRENT_BINARY_DIR}/${f})
        SET(srcs ${srcs} ${src})
        ENDFOREACH(f ${l})
        

        SET(build_srcs)
        SET(l ${nodist_${amname}_SOURCES} ${BUILT_SOURCES})
        FOREACH(f ${l})
        GET_FILENAME_COMPONENT(ext ${f} EXT)
        IF(ext STREQUAL .py)
        ELSE(ext STREQUAL .py)
        SET(build_srcs ${build_srcs} ${CMAKE_CURRENT_BINARY_DIR}/${f})
        ENDIF(ext STREQUAL .py)
        ENDFOREACH(f ${l})
        

        ADD_LIBRARY(${name} ${type} ${srcs})
        

        SET(var)
        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} SalomeIDLKernel)
        SET(targets ${targets} SalomeDS)
        SET(targets ${targets} SALOMEDSTest)
        SET(targets ${targets} SALOMEDS_Client_exe)
        SET(targets ${targets} SalomeIDLGEOM)
        SET(targets ${targets} GEOMEngine)
        SET(targets ${targets} MEDEngine)
        SET(targets ${targets} SMESHEngine)
        SET(targets ${targets} SMESH)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(var ${var} -DNOGDI)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(dir $ENV{F2CHOME})
        STRING(REPLACE "\\\\" "/" dir ${dir})
        SET(var ${var} -I${dir})
        SET(var ${var} -DF2C_BUILD)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        SET(var ${var} ${AM_CPPFLAGS})
        SET(var ${var} ${AM_CXXFLAGS})
        

	SET(var ${var} ${PLATFORM_CPPFLAGS})
	SET(var ${var} ${PTHREAD_CFLAGS})
	SET(var ${var} ${${amname}_CPPFLAGS})
	SET(var ${var} ${${amname}_CXXFLAGS})
	SET(var ${var} ${${amname}_CFLAGS})
        SET(vars)
        IF(WINDOWS)
        SET(vars ${vars} -include SALOMEconfig.h)
        SET(vars ${vars} -ftemplate-depth-32)
        SET(vars ${vars} -fPIC)
        SET(vars ${vars} -g)
        ENDIF(WINDOWS)
        SET(flags)
        FOREACH(f ${var})
        FOREACH(v ${vars})
        IF(f STREQUAL v)
        SET(f)
        ENDIF(f STREQUAL v)
        ENDFOREACH(v ${vars})
        SET(flags "${flags} ${f}")
        ENDFOREACH(f ${var})
        SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
        

        SET_TARGET_PROPERTIES(${name} PROPERTIES VERSION ${VERSION_INFO} SOVERSION ${SOVERSION_INFO})
        

        SET(libadd)
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(dir $ENV{F2CHOME})
        STRING(REPLACE "\\\\" "/" dir ${dir})
        SET(libadd ${libadd} ${dir}/LIBF77.lib)
        SET(libadd ${libadd} ${dir}/LIBI77.lib)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        SET(libs ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${${amname}_LIBADD} ${${amname}_LDADD} ${${amname}_LDFLAGS})
        FOREACH(lib SALOMEBasics SalomeBatch)
        IF(name STREQUAL lib)
        SET(libs ${libs} ${PTHREAD_LIBS})
        ENDIF(name STREQUAL lib)
        ENDFOREACH(lib SALOMEBasics SalomeBatch)
        

        FOREACH(lib ${libs})
        GET_FILENAME_COMPONENT(ext ${lib} EXT)
        IF(ext STREQUAL .la)
        GET_FILENAME_COMPONENT(lib ${lib} NAME_WE)
        STRING(REGEX REPLACE "^lib" "" lib ${lib})
        ENDIF(ext STREQUAL .la)
        SET(vars)
        SET(vars ${vars} -no-undefined)
        SET(vars ${vars} -lvtkWidgets)
        IF(WINDOWS)
        SET(vars ${vars} -module)
        SET(vars ${vars} -Wl,-E)
        SET(vars ${vars} -Xlinker)
        SET(vars ${vars} -export-dynamic)
        SET(vars ${vars} -lm)
        SET(vars ${vars} -lboost_thread)
        SET(vars ${vars} -lboost_signals)
        SET(vars ${vars} -pthread -lpthread -ldl)
        ENDIF(WINDOWS)
        FOREACH(v ${vars})
        IF(lib STREQUAL v)
        SET(lib)
        ENDIF(lib STREQUAL v)
        ENDFOREACH(v ${vars})
        SET(libadd ${libadd} ${lib})
        ENDFOREACH(lib ${libs})
        TARGET_LINK_LIBRARIES(${name} ${libadd})
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        IF(CMAKE_BUILD_TYPE STREQUAL Debug)
        SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
        ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

                SET(DEST lib/salome)
                

            IF(BEGIN_WITH_lib)
            INSTALL(TARGETS ${name} DESTINATION ${DEST})
            

            ELSE(BEGIN_WITH_lib)
            

            IF(WINDOWS)
            INSTALL(TARGETS ${name} DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome)
            IF(CMAKE_BUILD_TYPE STREQUAL Release)
            INSTALL(FILES ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION}/site-packages/salome/${name}.dll DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome RENAME ${name}.pyd)
            ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
            INSTALL(FILES ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION}/site-packages/salome/${name}.dll DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome RENAME ${name}_d.pyd)
            ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
            ELSE(WINDOWS)
            GET_TARGET_PROPERTY(version ${name} VERSION)
            GET_TARGET_PROPERTY(soversion ${name} SOVERSION)
            INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${name}.so.${version} DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome RENAME ${name}.so.${version})
            INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${name}.so.${version} DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome RENAME ${name}.so.${soversion})
            INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${name}.so.${version} DESTINATION lib/python${PYTHON_VERSION}/site-packages/salome RENAME ${name}.so)
            ENDIF(WINDOWS)
            

            ENDIF(BEGIN_WITH_lib)
            

        ENDFOREACH(amname ${lib_LTLIBRARIES})
        

        FOREACH(amname ${bin_PROGRAMS} ${check_PROGRAMS})
        

        SET(name "${amname}_exe")
        SET(srcs ${${amname}_SOURCES} ${dist_${amname}_SOURCES})
        SET(l ${nodist_${amname}_SOURCES})
        FOREACH(f ${l})
        SET(src ${CMAKE_CURRENT_BINARY_DIR}/${f})
        SET(srcs ${srcs} ${src})
        ENDFOREACH(f ${l})
        LIST(LENGTH srcs nb)
        IF(nb)
        ADD_EXECUTABLE(${name} ${srcs})
        

        SET(var)
        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} SalomeIDLKernel)
        SET(targets ${targets} SalomeDS)
        SET(targets ${targets} SALOMEDSTest)
        SET(targets ${targets} SALOMEDS_Client_exe)
        SET(targets ${targets} SalomeIDLGEOM)
        SET(targets ${targets} GEOMEngine)
        SET(targets ${targets} MEDEngine)
        SET(targets ${targets} SMESHEngine)
        SET(targets ${targets} SMESH)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(var ${var} -DNOGDI)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(dir $ENV{F2CHOME})
        STRING(REPLACE "\\\\" "/" dir ${dir})
        SET(var ${var} -I${dir})
        SET(var ${var} -DF2C_BUILD)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        SET(var ${var} ${AM_CPPFLAGS})
        SET(var ${var} ${AM_CXXFLAGS})
        

	SET(var ${var} ${PLATFORM_CPPFLAGS})
	SET(var ${var} ${PTHREAD_CFLAGS})
	SET(var ${var} ${${amname}_CPPFLAGS})
	SET(var ${var} ${${amname}_CXXFLAGS})
	SET(var ${var} ${${amname}_CFLAGS})
        SET(vars)
        IF(WINDOWS)
        SET(vars ${vars} -include SALOMEconfig.h)
        SET(vars ${vars} -ftemplate-depth-32)
        SET(vars ${vars} -fPIC)
        SET(vars ${vars} -g)
        ENDIF(WINDOWS)
        SET(flags)
        FOREACH(f ${var})
        FOREACH(v ${vars})
        IF(f STREQUAL v)
        SET(f)
        ENDIF(f STREQUAL v)
        ENDFOREACH(v ${vars})
        SET(flags "${flags} ${f}")
        ENDFOREACH(f ${var})
        SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
        

        SET(libadd)
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        SET(dir $ENV{F2CHOME})
        STRING(REPLACE "\\\\" "/" dir ${dir})
        SET(libadd ${libadd} ${dir}/LIBF77.lib)
        SET(libadd ${libadd} ${dir}/LIBI77.lib)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

        SET(libs ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${${amname}_LIBADD} ${${amname}_LDADD} ${${amname}_LDFLAGS})
        FOREACH(lib SALOMEBasics SalomeBatch)
        IF(name STREQUAL lib)
        SET(libs ${libs} ${PTHREAD_LIBS})
        ENDIF(name STREQUAL lib)
        ENDFOREACH(lib SALOMEBasics SalomeBatch)
        

            SET(libs ${libs} ${LDADD})
            

        FOREACH(lib ${libs})
        GET_FILENAME_COMPONENT(ext ${lib} EXT)
        IF(ext STREQUAL .la)
        GET_FILENAME_COMPONENT(lib ${lib} NAME_WE)
        STRING(REGEX REPLACE "^lib" "" lib ${lib})
        ENDIF(ext STREQUAL .la)
        SET(vars)
        SET(vars ${vars} -no-undefined)
        SET(vars ${vars} -lvtkWidgets)
        IF(WINDOWS)
        SET(vars ${vars} -module)
        SET(vars ${vars} -Wl,-E)
        SET(vars ${vars} -Xlinker)
        SET(vars ${vars} -export-dynamic)
        SET(vars ${vars} -lm)
        SET(vars ${vars} -lboost_thread)
        SET(vars ${vars} -lboost_signals)
        SET(vars ${vars} -pthread -lpthread -ldl)
        ENDIF(WINDOWS)
        FOREACH(v ${vars})
        IF(lib STREQUAL v)
        SET(lib)
        ENDIF(lib STREQUAL v)
        ENDFOREACH(v ${vars})
        SET(libadd ${libadd} ${lib})
        ENDFOREACH(lib ${libs})
        TARGET_LINK_LIBRARIES(${name} ${libadd})
        

        IF(WINDOWS)
        SET(targets)
        SET(targets ${targets} MEFISTO2D)
        FOREACH(target ${targets})
        IF(name STREQUAL ${target})
        IF(CMAKE_BUILD_TYPE STREQUAL Debug)
        SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
        ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
        ENDIF(name STREQUAL ${target})
        ENDFOREACH(target ${targets})
        ENDIF(WINDOWS)
        

            SET(DEST bin/salome)
            

            IF(WINDOWS)
            INSTALL(TARGETS ${name} DESTINATION ${DEST})
            INSTALL(FILES ${CMAKE_INSTALL_PREFIX}/${DEST}/${name}.exe DESTINATION ${DEST} RENAME ${amname}.exe)
            INSTALL(CODE "FILE(REMOVE ${CMAKE_INSTALL_PREFIX}/${DEST}/${name}.exe)")
            ELSE(WINDOWS)
            SET(PERMS)
            SET(PERMS ${PERMS} OWNER_READ OWNER_WRITE OWNER_EXECUTE)
            SET(PERMS ${PERMS} GROUP_READ GROUP_EXECUTE)
            SET(PERMS ${PERMS} WORLD_READ WORLD_EXECUTE)
            INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name} DESTINATION ${DEST} PERMISSIONS ${PERMS} RENAME ${amname})
            ENDIF(WINDOWS)
            

        ENDIF(nb)
        

        ENDFOREACH(amname ${bin_PROGRAMS} ${check_PROGRAMS})
        

            FOREACH(input ${IDL_FILES})
            STRING(REGEX REPLACE ".idl" "" base ${input})
            SET(src ${CMAKE_CURRENT_BINARY_DIR}/${base}SK.cc)
            SET(outputs ${src})
            SET(dynsrc ${CMAKE_CURRENT_BINARY_DIR}/${base}DynSK.cc)
            SET(outputs ${outputs} ${dynsrc})
            SET(inc ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
            SET(outputs ${outputs} ${inc})
            IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
            SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
            ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
            SET(input ${CMAKE_CURRENT_BINARY_DIR}/${input})
            ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
            SET(flags ${IDLCXXFLAGS} ${OMNIORB_IDLCXXFLAGS})
            STRING(REGEX MATCH "-bcxx" ISBCXX ${flags})
            IF(NOT ISBCXX)
            SET(flags -bcxx ${flags})
            ENDIF(NOT ISBCXX)
            ADD_CUSTOM_COMMAND(
            OUTPUT ${outputs}
            COMMAND ${OMNIORB_IDL} ${flags} ${input}
            MAIN_DEPENDENCY ${input}
            )
            install(FILES ${input} DESTINATION idl/salome)
            SET(IDL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
            install(FILES ${IDL_HEADER} DESTINATION include/salome)
            INSTALL(CODE "SET(IDL_FILE ${input})")
            INSTALL(CODE "SET(DIR lib/python${PYTHON_VERSION}/site-packages/salome)")
            INSTALL(CODE "SET(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})")
            INSTALL(CODE "SET(OMNIORB_IDL_PYTHON ${OMNIORB_IDL_PYTHON})")
            # --
            SET(flags)
            FOREACH(f ${IDLPYFLAGS})
            SET(flags "${flags} ${f}")
            ENDFOREACH(f ${IDLPYFLAGS})
            STRING(REGEX MATCH "-bpython" ISBPYTHON ${flags})
            IF(NOT ISBPYTHON)
            SET(flags "-bpython ${flags}")
            ENDIF(NOT ISBPYTHON)
            SET(IDLPYFLAGS ${flags})
            STRING(REPLACE "\\" "/" IDLPYFLAGS ${IDLPYFLAGS})
            INSTALL(CODE "SET(IDLPYFLAGS ${IDLPYFLAGS})")
            # --
            

                STRING(REPLACE "\\" "/" KERNEL_ROOT_DIR ${KERNEL_ROOT_DIR})
                INSTALL(SCRIPT ${KERNEL_ROOT_DIR}/salome_adm/cmake_files/install_python_from_idl.cmake)
                

            ENDFOREACH(input ${IDL_FILES})
            
