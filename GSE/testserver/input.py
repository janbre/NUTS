import socket

host = 'localhost'
port = 1234
size = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

while 1:
    command = raw_input('enter command: ')
    s.send(command)
    data = s.recv(size).split('\n')
    for line in data:
        print line
    print command, ' executed'

