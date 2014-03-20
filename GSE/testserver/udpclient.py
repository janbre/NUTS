import socket
import sys

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('localhost', 50000)
message = 'Hello world'

try:
    print >>sys.stderr, 'sending "%s"' % message
    sent = sock.sendto(message, server_address)
    print >>sys.stderr, 'waiting for data'
    data, server = sock.recvfrom(1024)
    print >>sys.stderr, 'received "%s"' % data
finally:
    print >>sys.stderr, 'closing socket'
    sock.close()

