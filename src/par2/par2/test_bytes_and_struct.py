import datetime
import struct

times = 10 ** 7

print('test do {} times by int.from_bytes() and int.to_bytes().'.format(times))
print()

for num_bytes_len in (1, 2, 4):
    print('num_bytes_len =', num_bytes_len)

    num_bytes = b'\x10' * num_bytes_len
    octets = len(num_bytes)

    s = datetime.datetime.now()
    for i in range(times):
        num = int.from_bytes(num_bytes, 'big')
    e = datetime.datetime.now()
    encode_seconds =  (e - s).total_seconds()
    print('encode {:.2f} seconds'.format(encode_seconds))

    s = datetime.datetime.now()
    for i in range(times):
        octet_bytes = int.to_bytes(num, octets, 'big')
    e = datetime.datetime.now()
    decode_seconds =  (e - s).total_seconds()
    print('decode {:.2f} seconds'.format(decode_seconds))
    print('num_bytes == octet_bytes', num_bytes == octet_bytes)
    print()

print('test do {} times by struct.pack() and struct.unpack().'.format(times))
print()

for num_bytes_len, fm in {1: 'B', 2: 'H', 4: 'L'}.items():
    fmt = '>{}'.format(fm)
    print('num_bytes_len =', num_bytes_len)

    num_bytes = b'\x10' * num_bytes_len

    s = datetime.datetime.now()
    for i in range(times):
        num = struct.unpack(fmt, num_bytes)
    e = datetime.datetime.now()
    encode_seconds =  (e - s).total_seconds()
    print('encode {:.2f} seconds'.format(encode_seconds))

    s = datetime.datetime.now()
    for i in range(times):
        octet_bytes = struct.pack(fmt, num[0])
    e = datetime.datetime.now()
    decode_seconds =  (e - s).total_seconds()
    print('decode {:.2f} seconds'.format(decode_seconds))
    print('num_bytes == octet_bytes', num_bytes == octet_bytes)
    print()
