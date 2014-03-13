import socket
import time

s = socket.socket()

host = ''
port = 1234

s.connect((host, port))

while True:
    data = s.recv(1024)
    for c in data:
        print c,
        time.sleep(0.05)
