import binascii

f           = '0b00110011'
buffer_no   = '0b11100011'
command     = '0b00001101'

packet = command + f + buffer_no

print int(packet, 2)
n = int(packet, 2)
print n
binascii.unhexlify('%x' % n)
