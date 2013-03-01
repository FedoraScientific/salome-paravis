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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)

set(TestMedReader_SRCS
    TestMedReader.cxx
)

set(TestMedPolyhedron_SRCS
    TestMedPolyhedron.cxx
)

set(TestMedReadPolyhedron_SRCS
    TestMedReadPolyhedron.cxx
)

set(TestMedDescendingPolyhedron_SRCS
    TestMedDescendingPolyhedron.cxx
)

set(TestMedReadDescendingPolyhedron_SRCS
    TestMedReadDescendingPolyhedron.cxx
)

set(TestMedWriteOcta12_SRCS
    TestMedWriteOcta12.cxx
)

set(TestMedParallelRead_SRCS
    TestMedParallelRead.cxx
)

set(TestMedParallelWrite_SRCS
    TestMedParallelWrite.cxx
)

add_executable(TestMedReader ${TestMedReader_SRCS})
add_executable(TestMedPolyhedron ${TestMedPolyhedron_SRCS})
add_executable(TestMedReadPolyhedron ${TestMedReadPolyhedron_SRCS})
add_executable(TestMedDescendingPolyhedron ${TestMedDescendingPolyhedron_SRCS})
add_executable(TestMedReadDescendingPolyhedron ${TestMedReadDescendingPolyhedron_SRCS})
add_executable(TestMedWriteOcta12 ${TestMedWriteOcta12_SRCS})

if(${HDF5_IS_PARALLEL})
  add_executable(TestMedParallelRead ${TestMedParallelRead_SRCS})
  add_executable(TestMedParallelWrite ${TestMedParallelWrite_SRCS})
  target_link_libraries(TestMedParallelRead vtkParallelCore ${MED_LIBRARIES})
  target_link_libraries(TestMedParallelWrite vtkParallelCore ${MED_LIBRARIES})
endif(${HDF5_IS_PARALLEL})

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../../module.cmake)

target_link_libraries(TestMedReader ${${vtk-module-test}_DEPENDS} ${MED_LIBRARIES})
target_link_libraries(TestMedPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedReadPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedDescendingPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedReadDescendingPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedWriteOcta12 ${MED_LIBRARIES})

