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

ADD_SUBDIRECTORY(VisuPrs)

SET(_demo_files
  demo_0_med.py
  demo_1_med.py
  demo_2_med.py
  demo_3_med.py
  demo_4_med.py
  demo_5_med.py
  demo_6_med.py
  demo_7_med.py
  demo_8_med.py
  demo_9_med.py
  demo_10_med.py
  demo_11_med.py
  demo_12_med.py
  demo_13_med.py
  demo_14_med.py
  demo_15_med.py
  )
 
INSTALL_AND_COMPILE_PYTHON_FILE("${_demo_files}" ${SALOME_INSTALL_SCRIPT_PYTHON})
  