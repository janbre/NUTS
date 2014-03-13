import socket
import select
import time
import thread
import sys

# different messages
HAL = '''ALL THESE WORLDS ARE YOURS EXCEPT EUROPA\nATTEMPT NO LANDING THERE\n'''

welcome = 'This is your reptilian overlords, what can we help you with today?\n'

help_request = 'Received request for help, transmitting hydrospanner manual'

help_text = '''Available commands:\nQuit:\tShut down connection\nHelp:\tPrints this message\nStart:\tBegin transmission\n'''

quit = 'Good day to you, madam/sir!\n'


# Setting up the socket stuffs
host = ''
port = 1337
gse_socket = socket.socket()
gse_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
gse_socket.bind((host, port))
gse_socket.listen(5)
inputs = [gse_socket]

def constant_transmission(thread_name, connection):
    while True:
        connection.send(HAL * 6)
        time.sleep(1)

while True:
    rs, ws, es = select.select(inputs, [], [])
    for r in rs:
        if r is gse_socket:
            client, address = gse_socket.accept()
            print 'Got connection from ', address
            inputs.append(client)
            client.send(welcome)
        else:
            try:
                data = client.recv(1024)
                disconnected = not data
            except socket.error:
                disconnected = True

            if disconnected:
                print client.getpeername(), 'disconnected'
                gse_socket.close()
                sys.exit()
            else:
                if data[:4].lower() == 'help':
                    print help_request
                    client.send(help_text)
                elif data[:4].lower() == 'quit':
                    print quit
                    gse_socket.close()
                    sys.exit()
                elif data[:5].lower() == 'start':
                    thread.start_new_thread(constant_transmission, ('transmission', client))
                else:
                    client.send(data)



