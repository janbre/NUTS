import socket
#header = '10011010'
header = '01000000'
ping = '010000'
host = 'localhost'
port = 50000
size = 1024
s = socket.socket()
s.connect((host,port))
s.send('@42!')
data = s.recv(size)
s.close()
data_header = '{0:08b}'.format(ord(data[0]))
packet_type = data_header[0:6]
request = data_header[6]
large = data_header[7]
print packet_type, request, large
if packet_type == ping:
    print 'received ping packet'
