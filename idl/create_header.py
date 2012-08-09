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

ffile = "vtkWrapIDL.h"
wfile = os.path.join(sys.argv[1], "wrapfiles.txt")

ffile_stream = open(ffile, "w")
ffile_stream.write('const char* wrapped_classes[] = {')
ffile_stream.write('\n')
wfile_stream = open(wfile)
for line in wfile_stream:
    ffile_stream.write('"'+line.split()[0]+'",')
    ffile_stream.write('\n')
    pass
wfile_stream.close()
ffile_stream.write('""')
ffile_stream.write('\n')
ffile_stream.write('};')
ffile_stream.write('\n')
ffile_stream.close()
