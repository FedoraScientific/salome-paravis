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

FIND_PACKAGE(ParaView REQUIRED)

SET(PARAVIEW_INCLUDES -I${PARAVIEW_INCLUDE_DIRS})

FIND_LIBRARY(QtPython QtPython PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(QVTK QVTK PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkClientServer vtkClientServer PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPVServerManager vtkPVServerManager PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(pqApplicationComponents pqApplicationComponents PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(pqComponents pqComponents PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(pqCore pqCore PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(pqWidgets pqWidgets PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPVClientServerCore vtkPVClientServerCore PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPVServerImplementation vtkPVServerImplementation  PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPVCommon vtkPVCommon PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPVVTKExtensions vtkPVVTKExtensions PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkViews vtkViews PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(pqApplicationComponents pqApplicationComponents  PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkGraphicsCS vtkGraphicsCS  PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkCommonCS vtkCommonCS  PATHS ${PARAVIEW_LIBRARY_DIRS} NO_DEFAULT_PATH)

SET(PARAVIEW_LIBS
  ${QtPython}
  ${QVTK}
  ${vtkClientServer}
  ${vtkGenericFiltering}
  ${vtkInfovis}
  ${vtkVolumeRendering}
  ${vtkPVServerManager}
  ${pqApplicationComponents}
  ${pqComponents}
  ${pqCore}
  ${pqWidgets}
  ${vtkPVClientServerCore}
  ${vtkPVServerImplementation}
  ${vtkPVVTKExtensions}
  ${vtkPVCommon}
  ${vtkViews}
  ${pqApplicationComponents}
  ${vtkGraphicsCS}
  ${vtkCommonCS}
  )