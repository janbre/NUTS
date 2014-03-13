import socket, select
import thread

# prefixes
prefix_HAL = 'HAL'
prefix_welcome = 'welcome'
prefix_help = 'man'
prefix_quit = 'quit'

welcome = '''Welcome to Alien Transmission Reception INC.
No ongoing transmissions. Would you like to receive one? (Y/N): '''

negative = '''Apparantly, E.T doesn\'t want to phone home today.
Closing conduit'''

connection_error = '''Unable to connect to server. Are you certain our
reptilian overlords are trying to send you a message?'''

shut_down = 'Shutting down'

host = 'localhost'
port = 1337

command = raw_input(welcome)

def receive_stream(name, command):
    f = open('received.txt', 'w')
    connection = socket.socket()
    try:
        connection.connect((host, port))
    except socket.error:
        print connection_error
    connection.send(command)
    data = connection.recv(1024)
    while data:
        if data.startswith(prefix_HAL):
            f.write(data)
        elif data.startswith(prefix_help):
            print data[len(prefix_help):]
        data = connection.recv(1024)

if command.lower() == 'n':
    print negative
    print shut_down
    quit()
elif command.lower() == 'y':
    radio_socket = socket.socket()
    try:
        radio_socket.connect((host,port))
        data = radio_socket.recv(1024)
        print data
    except:
        print 'Error:', connection_error
        print shut_down
        quit()
else:
    print 'Wrong answer! Shutting down!'
    quit()

while True:
    # TODO: open a new connection when start command is sent so that we don't mix data stream and other stuff
    command = raw_input('Enter command: ')
    if command == 'start':
        thread.start_new_thread(receive_stream, ('transmission', command))
    else:
        radio_socket.send(command)
        data = radio_socket.recv(1024)
        print data
