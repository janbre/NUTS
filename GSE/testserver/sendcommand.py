import messages
import sys

contact         = '0'
get_telemetry   = '1'
cam_take_pic    = '2'
req_pic_status  = '3'
req_pic         = '4'
quit            = 'q'
hilfe           = 'h'

help_text = '0:\tContact\n1:\tGet telemetry\n2:\tTake picture\n3:\tRequest pic status\n4:\tRequest pic\nh:\thelp\nq:\tquit'

buffer_no = 0

def build_contact_command():
    # TODO
    print 'contact!'
    return messages.request.CONTACT

def build_telemetry_command():
    #TODO:
    print 'telemetry!'
    return messages.request.GET_TELEMETRY

def build_take_pic_command():
    #TODO:
    global buffer_no
    print 'take pic! buffer no: ' + '{0:08b}'.format(buffer_no)
    buffer_no += 1
    return messages.request.CAM_TAKE_PIC

def build_req_pic_status_command():
    #TODO:
    print 'request pic status!'
    return messages.request.REQ_PIC_STATUS

def build_req_pic_command():
    #TODO:
    print 'request pic!'
    return messages.request.REQ_PIC


while True:
    command = raw_input('enter command: ')
    x = ''
    if command == contact:
        x = build_contact_command()
    elif command == get_telemetry:
        x = build_telemetry_command()
    elif command == cam_take_pic:
        x = build_take_pic_command()
    elif command == req_pic_status:
        x = build_req_pic_status_command()
    elif command == req_pic:
        x = build_req_pic_command()
    elif command == quit:
        confirmed = raw_input('Are you sure you want to quit? y/n: ')
        if confirmed == 'y':
            sys.exit()
    elif command == hilfe:
        print help_text
    else:
        print 'Unknown command. Type h for list of available commands'
    if x:
        print x


