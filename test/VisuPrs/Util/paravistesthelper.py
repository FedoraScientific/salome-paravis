import searchFreePort
import subprocess
import sys
import signal

args = {}
searchFreePort.searchFreePort(args)
port = args['port']

def timeout_handler(signum, frame):
    print "FAILED : timeout(" + sys.argv[1] + ") is reached"
    import killSalomeWithPort
    killSalomeWithPort.killMyPort(port)

signal.alarm(abs(int(sys.argv[1])-10))
signal.signal(signal.SIGALRM, timeout_handler)

subprocess.check_call([sys.executable]+sys.argv[2:])
