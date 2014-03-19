'''
All request/response types start with a leading 1 since Python likes
to ignore leading 0's
'''
class request:
    '''
    Composed of the MSG ID & Request bits
    https://www.ntnu.no/wiki/display/nuts/Message+Format
    '''
    CONTACT         = '10000110'
    TELEMETRY       = '10001010'
    IMAGE_CAPTURE   = '10001110' # parameters: buffer.no (8 bits), format (8 bits)
    IMAGE_STATUS    = '10010010' # parameters: buffer.no (8 bits)
    IMAGE_DOWNLOAD  = '10010110' # parameters: buffer.no (8 bits) start chunk (10 bits), delta (6 bits)
    PING            = '10011010' # parameters: 3 bytes data to be echoed
    PING_PAYLOAD    = '42!'
    EMPTY_PAYLOAD   = '\x00\x00\x00'

class response:
    '''
    Composed of the MSG ID & Response bits
    https://www.ntnu.no/wiki/display/nuts/Message+Format
    '''
    CONTACT         = '10000100'
    TELEMETRY       = '10001001'
    IMAGE_CAPTURE   = '10001100'
    IMAGE_STATUS    = '10010000' # taking pic (1 bit), ready for download (1 bit), size (22 bits)
    IMAGE_DOWNLOAD  = '10010101' # buffer.no (8 bits), chunk.no (16 bits), data = packet_size - header - CRC
    PING            = '10011001' # 3 bytes echoed data

class format:
    '''
    Picture format requested
    '''
    #TODO: enter REAL values here, not these bogus ones
    SMALL           = '10000001'
    LARGE           = '10000011'

