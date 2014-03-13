import socket
import time

s = socket.socket()

host = ''
port = 1234
s.bind((host, port))

s.listen(5)

while True:
    c, addr = s.accept()
    print 'Got connection from ', addr
    c.send('Thank you for connecting\n')
    c.send('Begin Transmission ...\n')
    time.sleep(1)
    while 1:
        c.send('ALL THESE WORLDS ARE YOURS, EXCEPT EUROPA\n')
        time.sleep(1)
        c.send('ATTEMPT NO LANDING THERE\n')
        time.sleep(1)

