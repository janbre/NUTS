import binascii
import commands
import messages
import sys

help_text = '0:\tContact\n1:\tGet telemetry\n2:\tTake picture\n3:\tRequest pic status\n4:\tRequest pic\n5:\tPing\nh:\tHelp\nq:\tQuit'

buffer_no = 0 # start at buffer value 0, increment when received image_capture ACK

def bytes_to_bits(data):
    bits = ''
    b = bytearray(data)
    for byte in b:
        bits += '{0:08b}'.format(byte)
    return bits


def build_contact_command():
    # TODO
    header = messages.request.CONTACT
    payload = messages.request.EMPTY_PAYLOAD
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_telemetry_command():
    #TODO:
    header = messages.request.TELEMETRY
    payload = messages.request.EMPTY_PAYLOAD
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_image_capture_command():
    #TODO:
    global buffer_no
    header = messages.request.IMAGE_CAPTURE
    payload = chr(buffer_no) + chr(int(messages.format.SMALL, 2)) + '\x00' # NULL byte to pad
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    buffer_no += 1 # this shouldn't be here, move to method checking for image_capture ACK
    return message

def build_image_status_command():
    #TODO:
    global buffer_no
    header = messages.request.IMAGE_STATUS
    payload = chr(buffer_no) + '\x00\x00'   # buffer.no as parameter, pad with NULL bytes
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_image_download_command():
    #TODO: how to decide on start chunk and delta
    global buffer_no
    header = messages.request.IMAGE_DOWNLOAD
    payload = chr(buffer_no) + '\x00\x00'   # buffer.no to download, NULL bytes as placeholder for start chunk and delta
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

def build_ping_command():
    #TODO:
    header = messages.request.PING
    payload = messages.request.PING_PAYLOAD
    header_as_bytes = chr(int(header,2))
    payload_as_bits = bytes_to_bits(payload)
    message = header_as_bytes + payload
    return message

while True:
    #TODO: allow parameters to be sent with commands (e.g. buffer to retrieve, format to take picture in)
    command = raw_input('enter command: ')
    x = ''
    if command == commands.CONTACT:
        x = build_contact_command()
    elif command == commands.TELEMETRY:
        x = build_telemetry_command()
    elif command == commands.IMAGE_CAPTURE:
        x = build_image_capture_command()
    elif command == commands.IMAGE_STATUS:
        x = build_image_status_command()
    elif command == commands.IMAGE_DOWNLOAD:
        x = build_image_download_command()
    elif command == commands.PING:
        x = build_ping_command()
    elif command == commands.QUIT:
        confirmed = raw_input('Are you sure you want to quit? y/n: ')
        if confirmed == 'y':
            sys.exit()
    elif command == commands.HELP:
        print help_text
    else:
        print 'Unknown command. Type h for list of available commands'
    if x:
        print x, len(x), 'buffer.no is: ' + str(buffer_no)
        header = chr(bytearray(x)[0])
        print bytes_to_bits(header)



