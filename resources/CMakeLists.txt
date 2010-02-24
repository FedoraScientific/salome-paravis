
        SET(VERSION_INFO 0.0.0)
        SET(SOVERSION_INFO 0)
        SET(SUBDIRS)
        SET(AM_CPPFLAGS)
        SET(AM_CXXFLAGS)
        SET(LDADD)
        

                SET(AM_CPPFLAGS ${AM_CPPFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                SET(AM_CXXFLAGS ${AM_CXXFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
                
#  Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
# -* Makefile *- 
#
# Author : Patrick GOLDBRONN (CEA)
# Date : 28/06/2001
#  Modified by : Alexander BORODIN (OCN) - autotools usage

# include $(top_srcdir)/adm_local/unix/make_common_starter.am

SET(dist_salomeres_DATA
    LightApp.xml
    SalomeApp.xml
    pqAppIcon22.png
    pqAppIcon16.png
    ParaViewFilters.xml
    ParaViewReaders.xml
    ParaViewSources.xml
    ParaViewWriters.xml
)

SET(nodist_salomeres_SCRIPTS
    PARAVISCatalog.xml
)


                SET(input ${CMAKE_CURRENT_SOURCE_DIR}/PARAVISCatalog.xml.in)
                

                SET(output ${CMAKE_CURRENT_BINARY_DIR}/PARAVISCatalog.xml)
                

                MESSAGE(STATUS "Creation of ${output}")
                CONFIGURE_FILE(${input} ${output})
                

                SET(input ${CMAKE_CURRENT_SOURCE_DIR}/SalomeApp.xml.in)
                

                SET(output ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml)
                

                MESSAGE(STATUS "Creation of ${output}")
                CONFIGURE_FILE(${input} ${output})
                

            FOREACH(dir ${SUBDIRS})
            IF(NOT dir STREQUAL .)
            ADD_SUBDIRECTORY(${dir})
            ENDIF(NOT dir STREQUAL .)
            ENDFOREACH(dir ${SUBDIRS})
            
FOREACH(f ${nodist_salomeres_SCRIPTS})

        SET(DEST share/salome/resources/paravis)
        

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
        
ENDFOREACH(f ${nodist_salomeres_SCRIPTS})
FOREACH(f ${dist_salomeres_DATA})

        SET(DEST share/salome/resources/paravis)
        

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
        
ENDFOREACH(f ${dist_salomeres_DATA})
