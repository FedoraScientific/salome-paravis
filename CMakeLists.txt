
            CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
            IF(COMMAND cmake_policy)
            cmake_policy(SET CMP0003 NEW)
            ENDIF(COMMAND cmake_policy)
            

                SET(WITH_BOOST $ENV{BOOSTDIR})
                SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR})
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPLATFORM.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPYTHON.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindOMNIORB.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPTHREADS.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindHDF5.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindBOOST.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindLIBXML2.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindSWIG.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindCPPUNIT.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindDOXYGEN.cmake)
                INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindKERNEL.cmake)
                

                    INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindPARAVIEW.cmake)
                    IF(EXISTS ${PARAVIEW_VTK_DIR}/VTKConfig.cmake)
                    SET(VTK_DIR ${PARAVIEW_VTK_DIR})
                    ENDIF(EXISTS ${PARAVIEW_VTK_DIR}/VTKConfig.cmake)
                    SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR})
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindCAS.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindQT4.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindOPENGL.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindVTK.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindQWT.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindSIPPYQT.cmake)
                    INCLUDE(${GUI_ROOT_DIR}/adm_local/cmake_files/FindGUI.cmake)
                    

                        SET(BUILD_PLUGINS 1)
                        SET(MED_ROOT_DIR $ENV{MED_ROOT_DIR})
                        SET(VISU_ROOT_DIR $ENV{VISU_ROOT_DIR})
                        INCLUDE(${MED_ROOT_DIR}/adm_local/cmake_files/FindMED.cmake)
                        IF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
                        INCLUDE(${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
                        SET(WITH_VISU 1)
                        ENDIF(EXISTS ${VISU_ROOT_DIR}/adm_local/cmake_files/FindVISU.cmake)
                        EXECUTE_PROCESS(COMMAND pvpython ${CMAKE_SOURCE_DIR}/getwrapclasses.py ${PARAVIEW_INCLUDE_DIRS})
                        

            SET(DOXYGEN_IS_OK 0)
            

                IF(WINDOWS)
                SET(CPPUNIT_IS_OK 0)
                ENDIF(WINDOWS)
                

        SET(VERSION_INFO 0.0.0)
        SET(SOVERSION_INFO 0)
        SET(SUBDIRS)
        SET(AM_CPPFLAGS)
        SET(AM_CXXFLAGS)
        SET(LDADD)
        

                SET(AM_CPPFLAGS ${AM_CPPFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                SET(AM_CXXFLAGS ${AM_CXXFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                
#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
# -* Makefile *-
#
# Author :
# Modified by : Alexander BORODIN (OCN) - autotools usage
#

# include $(top_srcdir)/adm_local/unix/make_common_starter.am

SET(ACLOCAL_AMFLAGS
    -I
    adm_local/unix/config_files
    -I
    ${KERNEL_ROOT_DIR}/salome_adm/unix/config_files
    -I
    ${GUI_ROOT_DIR}/adm_local/unix/config_files
)

SET(SUBDIRS
    idl
    adm_local
    resources
    src
    doc
    bin
)

SET(DISTCLEANFILES
    a.out
    aclocal.m4
    configure
    wrapfiles.txt
)

SET(salomeinclude_DATA
    PARAVIS_version.h
)

SET(EXTRA_DIST
    ${EXTRA_DIST}
    build_configure
    clean_configure
)

# include  $(top_srcdir)/wrapfiles.am
INCLUDE(wrapfiles.cmake)

# dist-hook:
# 	rm -rf `find $(distdir) -name CVS`

# dev_docs:
# 	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)

# docs: dev_docs


                SET(input ${CMAKE_CURRENT_SOURCE_DIR}/PARAVIS_version.h.in)
                

                SET(output ${CMAKE_CURRENT_BINARY_DIR}/PARAVIS_version.h)
                

                MESSAGE(STATUS "Creation of ${output}")
                CONFIGURE_FILE(${input} ${output})
                

            FOREACH(dir ${SUBDIRS})
            IF(NOT dir STREQUAL .)
            ADD_SUBDIRECTORY(${dir})
            ENDIF(NOT dir STREQUAL .)
            ENDFOREACH(dir ${SUBDIRS})
            
FOREACH(f ${salomeinclude_DATA})

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
        
ENDFOREACH(f ${salomeinclude_DATA})
