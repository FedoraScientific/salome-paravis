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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../..)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../..)
INCLUDE_DIRECTORIES(${MEDFILE_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${MED_ROOT_DIR}/include/salome)

SET( TestMedReader_SRCS TestMedReader.cxx )

SET( TestMedPolyhedron_SRCS TestMedPolyhedron.cxx )

SET( TestMedReadPolyhedron_SRCS TestMedReadPolyhedron.cxx )

SET( TestMedDescendingPolyhedron_SRCS TestMedDescendingPolyhedron.cxx )

SET( TestMedReadDescendingPolyhedron_SRCS TestMedReadDescendingPolyhedron.cxx )

SET( TestMedWriteOcta12_SRCS TestMedWriteOcta12.cxx )

SET( TestMedParallelRead_SRCS TestMedParallelRead.cxx )

SET( TestMedParallelWrite_SRCS TestMedParallelWrite.cxx )

ADD_EXECUTABLE(TestReading0 TestReading0.cxx)
TARGET_LINK_LIBRARIES(TestReading0 MEDLoaderForPV vtkCommonDataModel)

#ADD_EXECUTABLE(TestMedReader ${TestMedReader_SRCS})
#ADD_EXECUTABLE(TestMedPolyhedron ${TestMedPolyhedron_SRCS})
#ADD_EXECUTABLE(TestMedReadPolyhedron ${TestMedReadPolyhedron_SRCS})
#ADD_EXECUTABLE(TestMedDescendingPolyhedron ${TestMedDescendingPolyhedron_SRCS})
#ADD_EXECUTABLE(TestMedReadDescendingPolyhedron ${TestMedReadDescendingPolyhedron_SRCS})
#ADD_EXECUTABLE(TestMedWriteOcta12 ${TestMedWriteOcta12_SRCS})

IF(${HDF5_IS_PARALLEL})
  ADD_EXECUTABLE(TestMedParallelRead ${TestMedParallelRead_SRCS})
  ADD_EXECUTABLE(TestMedParallelWrite ${TestMedParallelWrite_SRCS})
  TARGET_LINK_LIBRARIES(TestMedParallelRead vtkParallelCore ${MED_LIBRARIES})
  TARGET_LINK_LIBRARIES(TestMedParallelWrite vtkParallelCore ${MED_LIBRARIES})
ENDIF(${HDF5_IS_PARALLEL})

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../../module.cmake)

#vtkIOMPIImage
#TARGET_LINK_LIBRARIES(TestMedReader ${${vtk-module-test}_DEPENDS} ${MEDFILE_C_LIBRARIES})
#TARGET_LINK_LIBRARIES(TestMedPolyhedron ${MEDFILE_C_LIBRARIES})
#TARGET_LINK_LIBRARIES(TestMedReadPolyhedron ${MEDFILE_C_LIBRARIES})
#TARGET_LINK_LIBRARIES(TestMedDescendingPolyhedron ${MEDFILE_C_LIBRARIES})
#TARGET_LINK_LIBRARIES(TestMedReadDescendingPolyhedron ${MEDFILE_C_LIBRARIES})
#TARGET_LINK_LIBRARIES(TestMedWriteOcta12 ${MEDFILE_C_LIBRARIES})

