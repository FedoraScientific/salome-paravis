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

import sys
import os

CONF_DIR = os.path.dirname(sys.argv[0])

content = ""
for filename in [
#    os.path.join(sys.argv[1], "hints"),
    os.path.join(CONF_DIR, "hints"),
    os.path.join(CONF_DIR, "hints_paravis"),
    ]:
    stream = open(filename)
    content += stream.read()
    stream.close()
    pass

filename = "hints"
stream = open(filename, "w")
stream.write(content)
stream.close()
