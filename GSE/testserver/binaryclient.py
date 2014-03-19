from GSEexceptions import PrefixError
from time import ctime
import os
import select
import socket
import strings
import sys
import thread

# TODO: logfile should just log errors and stuff
#       beacon data should be logged to beacon.log
#       housekeeping data should be logged to housekeeping.log
#       picture data should be logged to pictures

logfile = open('received.txt', 'a')
new_session = '###!!!###\n' + ctime() + '\n' + strings.Info.START_UP
logfile.write(new_session)
host = 'localhost'
port = 1337

started = False
t = False

def shutdown(reason):
    print strings.Info.SHUT_DOWN
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
            return strings.Prefix.QUIT
        else:
            raise PrefixError(data)
    except PrefixError as e:
        print 'Data contains no known prefix\n', e.value
    return ''

def parse_data(data):
    # TODO: Need to figure out format of data and parse it here
    # Return: type, data
    return None

def receive_stream(name, command):
    global t
    print t
    connection = socket.socket()
    try:
        connection.connect((host, port))
    except socket.error:
        print connection_error
    connection.send(command)
    data = connection.recv(408)
    while data:
        if not data:
            print 'no more data'
            break
        prefix = get_prefix(data)
        print 'prefix: ', prefix
        if prefix == strings.Prefix.HAL:
            logfile.write(ctime() + '\n' + data)
            os.fsync(logfile)
        elif prefix == strings.Prefix.QUIT:
            print 'finished'
            t = False
        data = connection.recv(408)

command = raw_input(strings.Info.WELCOME)
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
    if command.lower() == 'start' and started == False and t == False:
        started = True
        t = True
        thread.start_new_thread(receive_stream, ('transmission', command))
    elif command.lower() == 'start' and started == True and t == True:
        print 'Transmission already started', t, started
    elif command.lower() == 'start' and t == False:
        print 'resuming transmission'
        thread.start_new_thread(receive_stream, ('transmission', command))
    elif command.lower() == 'quit':
        print 'Closing conduit...'
        shutdown(strings.Reason.USER_QUIT)
    else:
        radio_socket.send(command.lower())
        data = radio_socket.recv(1024)
        prefix = get_prefix(data)
        print 'Sent command: ' + data[len(prefix):]
