import socket
import sys

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
addr = ('localhost', int(sys.argv[1]))
print >>sys.stderr, 'listening on %s port %s' % addr
sock.bind(addr)

while True:
    buf, raddr = sock.recvfrom(4096)
    print >>sys.stderr, buf
    if buf:
        sent = sock.sendto(buf, raddr)
