import struct

x = int(input())
b = x.to_bytes(8, byteorder='big')
print(hex(x), struct.unpack('!d', b)[0])