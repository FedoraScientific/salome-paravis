import searchFreePort
import subprocess
import sys
import signal
import killSalomeWithPort

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
