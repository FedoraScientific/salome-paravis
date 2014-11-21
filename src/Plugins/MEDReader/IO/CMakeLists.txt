# Copyright (C) 2010-2014  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

INCLUDE_DIRECTORIES(
  ${MEDFILE_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${MED_ROOT_DIR}/include/salome
  )

IF(HDF5_IS_PARALLEL)
  ADD_DEFINITIONS("-DMEDREADER_USE_MPI")
ENDIF(HDF5_IS_PARALLEL)

SET(MEDReader_CLASSES vtkMEDReader vtkExtractGroup vtkELNOMeshFilter vtkELNOSurfaceFilter vtkELNOFilter vtkExtractCellType)

SET(MEDReader_SRCS)
SET(MEDReader_HDRS)

FOREACH(class ${MEDReader_CLASSES})
    SET(MEDReader_SRCS ${MEDReader_SRCS} ${class}.cxx)
    SET(MEDReader_HDRS ${MEDReader_HDRS} ${class}.h)
ENDFOREACH(class)

ADD_LIBRARY(MEDLoaderForPV SHARED MEDFileFieldRepresentationTree.cxx MEDTimeReq.cxx MEDUtilities.cxx vtkGenerateVectors.cxx)

IF(HDF5_IS_PARALLEL)
  TARGET_LINK_LIBRARIES(MEDLoaderForPV vtkCommonCore vtkCommonDataModel ${MED_paramedloader} ${MEDFILE_C_LIBRARIES})
ELSE(HDF5_IS_PARALLEL)
  TARGET_LINK_LIBRARIES(MEDLoaderForPV vtkCommonCore vtkCommonDataModel ${MED_medloader} ${MEDFILE_C_LIBRARIES})
ENDIF(HDF5_IS_PARALLEL)

VTK_MODULE_LIBRARY(vtkMEDReader ${MEDReader_SRCS})
TARGET_LINK_LIBRARIES(vtkMEDReader vtkPVVTKExtensionsRendering vtkRenderingFreeType vtkRenderingOpenGL vtkRenderingFreeTypeOpenGL vtkInteractionStyle vtkFiltersCore vtkFiltersGeneral MEDLoaderForPV ${MEDFILE_C_LIBRARIES}) 

INSTALL(
  TARGETS vtkMEDReader MEDLoaderForPV
  RUNTIME DESTINATION ${SALOME_INSTALL_LIBS}
  LIBRARY DESTINATION ${SALOME_INSTALL_LIBS}
  ARCHIVE DESTINATION ${SALOME_INSTALL_LIBS}
  )

INSTALL(FILES ${MEDReader_HDRS} DESTINATION include/paraview)
