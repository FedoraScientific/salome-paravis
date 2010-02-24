
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
#  File   : Makefile.in
#  Author : Vitaly Smetannikov
#  Module : PARAVIS
#  $Header: 
#
# include $(top_srcdir)/adm_local/unix/make_common_starter.am

SET(lib_LTLIBRARIES
    libPARAVISEngine.la
)

SET(salomeinclude_HEADERS
    PARAVIS_Engine_i.hh
)

SET(dist_libPARAVISEngine_la_SOURCES
    PARAVIS_Engine_i.cc
)

SET(libPARAVISEngine_la_CPPFLAGS
    ${KERNEL_CXXFLAGS}
    ${CORBA_CXXFLAGS}
    ${CORBA_INCLUDES}
    -I${CMAKE_BINARY_DIR}/salome_adm/unix
    -I${CMAKE_BINARY_DIR}/idl
)

SET(libPARAVISEngine_la_LDFLAGS
    ${KERNEL_LDFLAGS}
    ${SalomeIDLKernel}
    ${SalomeNS}
    ${SalomeContainer}
    ${CORBA_LIBS}
    ${CMAKE_BINARY_DIR}/idl/libSalomeIDLPARAVIS.la
)


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
        
FOREACH(f ${salomeinclude_HEADERS})

        SET(DEST include/salome)
        

        STRING(COMPARE EQUAL ${f} SALOMEconfig.h.in test_SALOMEconfig.h.in)
        IF(test_SALOMEconfig.h.in)
        INSTALL(FILES SALOMEconfig.ref.in DESTINATION ${DEST} RENAME SALOMEconfig.h.in)
        ELSE(test_SALOMEconfig.h.in)
        SET(dummy dummy-NOTFOUND)
        MARK_AS_ADVANCED(dummy)
        # FILE(REMOVE ${CMAKE_INSTALL_PREFIX}/${DEST}/${f})
        FIND_FILE(dummy ${f} PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
        IF(dummy)
        

            GET_FILENAME_COMPONENT(ext ${f} EXT)
            IF(ext STREQUAL .py)
            IF(DEST STREQUAL bin/salome)
            SET(PERMS)
            SET(PERMS ${PERMS} OWNER_READ OWNER_WRITE OWNER_EXECUTE)
            SET(PERMS ${PERMS} GROUP_READ GROUP_EXECUTE)
            SET(PERMS ${PERMS} WORLD_READ WORLD_EXECUTE)
            INSTALL(FILES ${f} DESTINATION ${DEST} PERMISSIONS ${PERMS})
            ELSE(DEST STREQUAL bin/salome)
            INSTALL(FILES ${f} DESTINATION ${DEST})
            ENDIF(DEST STREQUAL bin/salome)
            ELSE(ext STREQUAL .py)
            INSTALL(FILES ${f} DESTINATION ${DEST})
            ENDIF(ext STREQUAL .py)
            

        ELSE(dummy)
        GET_FILENAME_COMPONENT(ext ${f} EXT)
        IF(ext STREQUAL .qm)
        STRING(REGEX REPLACE .qm .ts input ${f})
        

            SET(input ${CMAKE_CURRENT_SOURCE_DIR}/resources/${input})
            

        SET(output ${CMAKE_CURRENT_BINARY_DIR}/${f})
        # ADD_CUSTOM_COMMAND(
        # OUTPUT ${output}
        # COMMAND ${QT_LRELEASE_EXECUTABLE} ${input} -qm ${output}
        # MAIN_DEPENDENCY ${input}
        # )
        EXECUTE_PROCESS(COMMAND ${QT_LRELEASE_EXECUTABLE} ${input} -qm ${output})
        ENDIF(ext STREQUAL .qm)
        INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f} DESTINATION ${DEST})
        ENDIF(dummy)
        ENDIF(test_SALOMEconfig.h.in)
        

        GET_FILENAME_COMPONENT(ext ${f} EXT)
        IF(ext STREQUAL .py)
        INSTALL(CODE "SET(PYTHON_FILE ${f})")
        INSTALL(CODE "SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})")
        INSTALL(CODE "SET(DEST ${DEST})")
        INSTALL(CODE "SET(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})")
        

            STRING(REPLACE "\\" "/" KERNEL_ROOT_DIR ${KERNEL_ROOT_DIR})
            INSTALL(SCRIPT ${KERNEL_ROOT_DIR}/salome_adm/cmake_files/install_and_compile_python_file.cmake)
            

        ENDIF(ext STREQUAL .py)
        
ENDFOREACH(f ${salomeinclude_HEADERS})
