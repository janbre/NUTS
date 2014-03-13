import socket, select

welcome = '''Welcome to Alien Transmission Reception INC.
No ongoing transmissions. Would you like to receive one? (Y/N): '''

negative = '''Apparantly, E.T doesn\'t want to phone home today.
Closing conduit'''

connection_error = '''Unable to connect to server. Are you certain our
reptilian overlords are trying to send you a message?'''

shut_down = 'Shutting down'

host = 'localhost'
port = 1234

command = raw_input(welcome)

if command.lower() == 'n':
    print negative
    print shut_down
    quit()
elif command.lower() == 'y':
    radio_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        radio_socket.connect((host,port))
    except:
        print 'Error:', connection_error
        print shut_down
        quit()
else:
    print 'Wrong answer! Shutting down!'
    quit()
