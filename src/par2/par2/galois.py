# Copyright 2011 梅どぶろく(umedoblock)
import math
import sys

print(sys.argv)
if len(sys.argv) == 1:
    raise ValueError('you must use 1 argment')

n_bits = int(sys.argv[1])
poly = 1 << n_bits
field_max = poly - 1

digits = int(math.log(19, 10)) + 1
fmt = '{:' + str(digits) + 'd}, 0b{:b}'
# print('fmt =', fmt)
print('n_bits =', n_bits)
print('galois_field =')

for galois_field in range(1, poly, 2):
# galois_field = 25
    galois_field += poly
    # set LSB
    possible = set()
    bit_pattern = 1

    for n in range(field_max):
        if bit_pattern in possible:
            break
        possible.add(bit_pattern)
        bit_pattern <<= 1
        if bit_pattern & poly:
            bit_pattern ^= galois_field

    if len(possible) == field_max:
        print(fmt.format(galois_field, galois_field))
      # print('{:4d}, 0b{:b}'.format(galois_field, galois_field))
      # print('possible =', possible)
      # print()
print()
