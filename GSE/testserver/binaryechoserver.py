import socket

host = ''
port = 50000
backlog = 5
size = 1024
s = socket.socket()
s.bind((host,port))
s.listen(5)
while True:
    client, address = s.accept()
    data = client.recv(size)
    if data:
        client.send(data)
    client.close()
