import socket, select
import thread
import sys
from time import ctime
from GSEexceptions import PrefixError
import strings

# prefixes
prefix_HAL = 'HAL'
prefix_welcome = 'welcome'
prefix_help = 'help'
prefix_quit = 'quit'

welcome = '''Welcome to Alien Transmission Reception INC.
No ongoing transmissions. Would you like to receive one? (Y/N): '''

negative = '''Apparantly, E.T doesn\'t want to phone home today.
Closing conduit'''

connection_error = '''Unable to connect to server. Are you certain our reptilian overlords are trying to send you a message?'''

shut_down = 'Shutting down'

reason_user_quit = 'User quit\n'
reason_unsupported_command = 'Unsupported command\n'
reason_connection_error = 'Connection error\n'

logfile = open('received.txt', 'a')
new_session = '###!!!###\n' + ctime() + '\n' + strings.Info.START_UP
logfile.write(new_session)
host = 'localhost'
port = 1337

started = False

def shutdown(reason):
    print shut_down
    #logfile.write(ctime() + '\n' +  shut_down + ' REASON: ' + reason)
    logfile.write(ctime() + '\n' + strings.Info.SHUT_DOWN + reason)
    logfile.close()
    sys.exit()

def get_prefix(data):
    try:
        if data.startswith(strings.Prefix.HAL):
            return strings.Prefix.HAL
        elif data.startswith(strings.Prefix.WELCOME):
            return strings.Prefix.WELCOME
        elif data.startswith(strings.Prefix.HELP):
            return strings.Prefix.HELP
        elif data.startswith(strings.Prefix.QUIT):
            return strings.Prefix.HELP
        else:
            raise PrefixError(data)
    except PrefixError as e:
        print 'Data contains no known prefix\n', e.value
    return ''

def receive_stream(name, command):
    connection = socket.socket()
    try:
        connection.connect((host, port))
    except socket.error:
        print connection_error
    connection.send(command)
    data = connection.recv(1024)
    while data:
        if get_prefix(data) == strings.Prefix.HAL:
            logfile.write(ctime() + '\n' + data)
        #elif get_prefix(data) == 'help':
        #    print data[len(prefix_help):]
        data = connection.recv(1024)

command = raw_input(welcome)
if command.lower() == 'n':
    print negative
    shutdown(strings.Reason.USER_QUIT)
elif command.lower() == 'y':
    print 'Opening conduit...'
    radio_socket = socket.socket()
    try:
        radio_socket.connect((host,port))
        data = radio_socket.recv(1024)
        print 'Transmission starting...'
        print data[len(get_prefix(data)):]
    except:
        print 'Error:', connection_error
        shutdown(strings.Reason.CONNECTION_ERROR)
else:
    print 'Wrong answer! Shutting down!'
    shutdown(reason_unsupported_command)

while True:
    command = raw_input('Enter command: ')
    if command.lower() == 'start' and started == False:
        started = True
        thread.start_new_thread(receive_stream, ('transmission', command))
    elif command.lower() == 'start' and started == True:
        print 'Transmission already started'
    elif command.lower() == 'quit':
        print 'Closing conduit...'
        shutdown(strings.Reason.USER_QUIT)
    else:
        radio_socket.send(command.lower())
        data = radio_socket.recv(1024)
        prefix = get_prefix(data)
        print 'Sent command: ' + data[len(prefix):]
