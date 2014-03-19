import socket
import select
import time
import thread
import sys
import strings

# Setting up the socket stuffs
host = ''
port = 1337
gse_socket = socket.socket()
gse_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
gse_socket.bind((host, port))
gse_socket.listen(5)
inputs = [gse_socket]
packet = '100000011111111110101010'

def constant_transmission(thread_name, connection):
    #TODO: send X messages, then stop... see if new start-command works
    for i in range(3):
        connection.send(strings.Prefix.HAL + (strings.Message.HAL * 6))
        time.sleep(1)
    connection.send(strings.Prefix.QUIT)
    print 'sent quit'

while True:
    rs, ws, es = select.select(inputs, [], [])
    for r in rs:
        if r is gse_socket:
            client, address = gse_socket.accept()
            print 'Got connection from ', address
            inputs.append(client)
            client.send(strings.Prefix.WELCOME + strings.Message.WELCOME)
        else:
            try:
                data = r.recv(1024)
                disconnected = not data
            except socket.error:
                disconnected = True

            if disconnected:
                print r.getpeername(), 'disconnected'
                gse_socket.close()
                sys.exit()
            else:
                if data[:4] == 'help':
                    print strings.Message.HELP_TEXT
                    r.send(strings.Prefix.HELP + strings.Message.HELP_TEXT)
                elif data[:4] == 'quit':
                    print strings.Message.QUIT
                    gse_socket.close()
                    sys.exit()
                elif data[:5] == 'start':
                    thread.start_new_thread(constant_transmission, ('transmission', r))
                else:
                    r.send(data)



