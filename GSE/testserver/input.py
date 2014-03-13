import socket

host = 'localhost'
port = 1234
size = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
f = open('received.txt', 'w')

while 1:
    command = raw_input('enter command: ')
    s.send(command)
    '''data = s.recv(size).split('\n')
    while(data):
        for line in data:
            print line
            f.write(line + '\n')
        print command, ' executed'
        data = s.recv(1024).split('\n')
        if data == None:
            print "haha"'''
    data = s.recv(size)
    print data

