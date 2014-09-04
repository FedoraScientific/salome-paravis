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

ADD_SUBDIRECTORY(VisuPrs)
ADD_SUBDIRECTORY(Data)

SET(_demo_files
  demo0.py
  demo1.py
  demo2.py
  demo3.py
  demo4.py
  demo5.py
  demo6.py
  demo7.py
  demo8.py
  demo9.py
  demo10.py
  demo11.py
  demo12.py
  demo_14_med.py
  demo_15_med.py
  )
 
INSTALL_AND_COMPILE_PYTHON_FILE("${_demo_files}" ${SALOME_INSTALL_SCRIPT_PYTHON})
  
