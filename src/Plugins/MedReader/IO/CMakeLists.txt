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

set(MedReader_CLASSES
    vtkMedReader
    vtkMedDriver
    vtkMedDriver30
    vtkMedFactory
    vtkMedFile
    vtkMedMesh
    vtkMedGrid
    vtkMedRegularGrid
    vtkMedPolarGrid
    vtkMedCartesianGrid
    vtkMedCurvilinearGrid
    vtkMedUnstructuredGrid
    vtkMedFamily
    vtkMedGroup
    vtkMedField
    vtkMedEntityArray
    vtkMedProfile
    vtkMedLocalization
    vtkMedFieldOverEntity
    vtkMedFieldStep
    vtkMedUtilities
    #vtkMedString
    vtkMedIntArrayInternal
    vtkMedIntArray
    vtkMedFamilyOnEntity
    vtkMedInterpolation
    vtkMedFraction
    vtkMedFilter
    vtkMedFieldOnProfile
    vtkExtractGroup
    vtkMedSelection
    vtkMedLink
    vtkMedFamilyOnEntityOnProfile
    vtkMedConstantAttribute
    vtkMedVariableAttribute
    vtkMedStructElement
    vtkGenerateStructElement
)

set(MedReader_SRCS)
set(MedReader_HDRS)

foreach(class ${MedReader_CLASSES})
    set(MedReader_SRCS ${MedReader_SRCS} ${class}.cxx)
    set(MedReader_HDRS ${MedReader_HDRS} ${class}.h)
endforeach(class)

set_source_files_properties(
  vtkMedGrid
  vtkMedRegularGrid
  vtkMedUtilities
  PROPERTIES ABSTRACT 1
)

set_source_files_properties(
  vtkMedIntArrayInternal
  vtkMedUtilities
  PROPERTIES WRAP_EXCLUDE 1
)

# this adds target_link_libraries dependencies on all modules listed under
# DEPENDS in module.cmake
vtk_module_library(vtkMedReader ${MedReader_SRCS})

# Introducing selection for parallel compilation
# CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/IO/config.h.cmake ${CMAKE_SOURCE_DIR}/IO/config.h )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

target_link_libraries(vtkMedReader ${MED_LIBRARIES})

#set(MedReader_BUILD_Testing ${VTK_BUILD_TESTING} CACHE BOOL "Build tests for the MedReader filter")

#if(MedReader_BUILD_Testing)
#    add_subdirectory(Testing)
#endif(MedReader_BUILD_Testing)

install(
    TARGETS vtkMedReader
    RUNTIME DESTINATION lib/salome
    LIBRARY DESTINATION lib/salome
    ARCHIVE DESTINATION lib/salome
    )

install(FILES ${MedReader_HDRS} DESTINATION include)
        
# -----------------------------------------------------------------------------
# This make it easy for other projects to get the list of files etc. in this
# kit.
# -----------------------------------------------------------------------------
# needed by vtkExportKit.cmake

#include(${ParaView_CMAKE_DIR}/VTKMono/vtkExportKit.cmake)

#  #include(${VTK_CMAKE_DIR}/vtkExportKit.cmake)
#IF(NOT VTK_INSTALL_NO_DEVELOPMENT)
#  STRING(REGEX REPLACE "^/" "" VTK_INSTALL_PACKAGE_DIR_CM24 "${VTK_INSTALL_PACKAGE_DIR}")
#ENDIF(NOT VTK_INSTALL_NO_DEVELOPMENT)

#set(VTK_INSTALL_PACKAGE_DIR_CM24 ${VTK_MODULES_DIR})
#vtk_export_kit2(
#  "MedReader_IO" 
#  "MEDREADER_IO" 
#  ${CMAKE_CURRENT_BINARY_DIR} 
#  "${MedReader_SRCS}"
#)
