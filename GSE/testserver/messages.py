class request:
    '''
    Composed of the MSG ID & Request bits
    https://www.ntnu.no/wiki/display/nuts/Message+Format
    '''
    CONTACT         = '00000110'
    GET_TELEMETRY   = '00001010'
    CAM_TAKE_PIC    = '00001110' # parameters: format (8 bits), buffer.no (8 bits)
    REQ_PIC_STATUS  = '00010010' # parameters: buffer.no (8 bits)
    REQ_PIC         = '00010110' # parameters: start chunk (10 bits), delta (6 bits), buffer.no (8 bits)

class response:
    '''
    Composed of the MSG ID & Response bits
    https://www.ntnu.no/wiki/display/nuts/Message+Format
    '''
    CONTACT         = '00000100'
    GET_TELEMETRY   = '00001001'
    CAM_TAKE_PIC    = '00001100'
    REQ_PIC_STATUS  = '00010000' # taking pic (1 bit), ready for download (1 bit), size (22 bits)
    REQ_PIC         = '00010101' # chunk.no (16 bits), buffer.no (8 bits) data = packet_size - header - CRC

class format:
    '''
    Picture format requested
    '''
    #TODO:

