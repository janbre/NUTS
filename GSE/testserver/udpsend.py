import binascii
import commands
import messages
import sys
import socket
import thread
import time

### Some 'constants'
help_text = '0:\tContact\n1:\tGet telemetry\n2:\tTake picture\n3:\tRequest pic status\n4:\tRequest pic\n5:\tPing\nh:\tHelp\nq:\tQuit'

host = 'localhost'
port = 1337
packet_size = 1024

buffer_no = 0 # start at buffer value 0, increment when received image_capture ACK

def bytes_to_bits(data):
    bits = ''
    b = bytearray(data)
    for byte in b:
        bits += '{0:08b}'.format(byte)
    return bits


def build_contact_command():
    header = messages.request.CONTACT
    payload = messages.request.EMPTY_PAYLOAD
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_telemetry_command():
    header = messages.request.TELEMETRY
    payload = messages.request.EMPTY_PAYLOAD
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_image_capture_command():
    global buffer_no
    header = messages.request.IMAGE_CAPTURE
    payload = chr(buffer_no) + chr(int(messages.format.SMALL, 2)) + '\x00' # NULL byte to pad
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    buffer_no += 1 # this shouldn't be here, move to method checking for image_capture ACK
    return message

def build_image_status_command():
    #TODO: the image buffer.no we want to check must be added as payload. Now there's only NULL
    global buffer_no
    header = messages.request.IMAGE_STATUS
    payload = chr(buffer_no) + '\x00\x00'   # buffer.no as parameter, pad with NULL bytes
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_image_download_command():
    #TODO: how to decide on start chunk and delta
    global buffer_no
    header = messages.request.IMAGE_DOWNLOAD
    payload = chr(buffer_no) + '\x00\x00'   # buffer.no to download, NULL bytes as placeholder for start chunk and delta
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_ping_command():
    header = messages.request.PING
    payload = messages.request.PING_PAYLOAD
    header_as_bytes = chr(int(header, 2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def receive_data(name, sock):
    '''
    Receive data from radio. At the moment just prints the data as the
    decimal values of the bytes along with the bit representation
    '''
    while True:
        data, server = sock.recvfrom(packet_size)
        byte_rep = bytearray(data)
        print 'data received: ' + data
        print 'as bytearray:',
        for byte in byte_rep:
            print byte,
        print
        print 'as bits:',
        for b in byte_rep:
            print '{0:08b}'.format(b),
        print

# create UDP-socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = (host, port)
thread.start_new_thread(receive_data, ('thread1', sock))

while True:
    #TODO: allow parameters to be sent with commands (e.g. buffer to retrieve, format to take picture in)
    user_input = raw_input('enter command: ')
    command = ''
    if user_input == commands.CONTACT:
        command = build_contact_command()
    elif user_input == commands.TELEMETRY:
        command = build_telemetry_command()
    elif user_input == commands.IMAGE_CAPTURE:
        command = build_image_capture_command()
    elif user_input == commands.IMAGE_STATUS:
        command = build_image_status_command()
    elif user_input == commands.IMAGE_DOWNLOAD:
        command = build_image_download_command()
    elif user_input == commands.PING:
        command = build_ping_command()
    elif user_input == commands.QUIT:
        confirmed = raw_input('Are you sure you want to quit? y/n: ')
        if confirmed == 'y':
            sys.exit()
    elif user_input == commands.HELP:
        print help_text
    else:
        print 'Unknown command. Type h for list of available commands'
    if command:
        #print x, len(x), 'buffer.no is: ' + str(buffer_no)
        #header = chr(bytearray(x)[0])
        #print bytes_to_bits(header)
        try:
            sent = sock.sendto(command, server_address)
            print 'sent %s bytes to radio' % sent
            print 'yay!'
        except:
            print 'something went wrong'
    time.sleep(0.2) # just so receiving thread finishes it's job before we print a new "enter command"
