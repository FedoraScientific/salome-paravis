# Copyright (C) 2013-2014  CEA/DEN, EDF R&D
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

import searchFreePort
import subprocess
import sys, os
import signal
import killSalomeWithPort

## TEMP >>> ###
if not os.getenv("OMNIORB_USER_PATH"):
    os.environ["OMNIORB_USER_PATH"] = os.path.realpath(os.path.expanduser('~'))
    pass
## <<< TEMP ###


args = {}
searchFreePort.searchFreePort(args)
port = args['port']

def timeout_handler(signum, frame):
    print "FAILED : timeout(" + sys.argv[1] + ") is reached"
    killSalomeWithPort.killMyPort(port)
    exit(1)

signal.alarm(abs(int(sys.argv[1])-10))
signal.signal(signal.SIGALRM, timeout_handler)

res = 1
try:
    res = subprocess.check_call([sys.executable]+sys.argv[2:])
except:
    pass

killSalomeWithPort.killMyPort(port)

exit(res)
