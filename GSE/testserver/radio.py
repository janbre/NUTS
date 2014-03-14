import socket
import select
import time
import thread
import sys

# different message prefixes
prefix_HAL = 'HAL'
prefix_welcome = 'welcome'
prefix_help_text = 'help'
prefix_quit = 'quit'

# different messages
message_HAL = '''ALL THESE WORLDS ARE YOURS EXCEPT EUROPA\nATTEMPT NO LANDING THERE\n'''

message_welcome = 'This is your reptilian overlords, what can we help you with today?\n'

message_help_request = 'Received request for help, transmitting hydrospanner manual'

message_help_text = '''Available commands:\nQuit:\tShut down connection\nHelp:\tPrints this message\nStart:\tBegin transmission\n'''

message_quit = 'Good day to you, madam/sir!\n'


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
        connection.send(prefix_HAL + (message_HAL * 6))
        time.sleep(1)

while True:
    rs, ws, es = select.select(inputs, [], [])
    for r in rs:
        if r is gse_socket:
            client, address = gse_socket.accept()
            print 'Got connection from ', address
            inputs.append(client)
            client.send(prefix_welcome + message_welcome)
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
                    print message_help_request
                    r.send(prefix_help_text + message_help_text)
                elif data[:4] == 'quit':
                    print message_quit
                    gse_socket.close()
                    sys.exit()
                elif data[:5] == 'start':
                    thread.start_new_thread(constant_transmission, ('transmission', r))
                else:
                    r.send(data)



