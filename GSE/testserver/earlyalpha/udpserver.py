import socket
import sys

host = 'localhost'
port = 1337

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = (host, port)
print >>sys.stderr, 'starting'
sock.bind(server_address)

while True:
    print >>sys.stderr, '\nwaiting for messagge'
    data, address = sock.recvfrom(1024)

    print >>sys.stderr, 'received %s bytes from %s' %(len(data), address)
    print >>sys.stderr, data

    if data:
        sent = sock.sendto(data, address)
        print >>sys.stderr, 'sent %s bytes back to %s' % (sent, address)
        print 'received data' + data

