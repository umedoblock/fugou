import sys
sys.path.insert(0, 'build/lib.linux-i686-3.2')
from _debug_gcdext import gcdext
import cProfile

def run():
    n = 1
    n = 100000
    for i in range(n):
        a = 0xf85744946adc388cb788e3fda5909957e29aeeb06e690495ba2d6a06c040df4d1
        b = 0x3008c0921487ab2efa77950977e5c8a7c4fcd70909296ca07b521dd91213153ab
        gcd, x, y = gcdext(a, b)
    print('a * x - b * y =', a * x - b * y)

cProfile.run('run()')

# a = 0x3008c0921487ab2efa77950977e5c8a7c4fcd70909296ca07b521dd91213153ab
# b = 0xf85744946adc388cb788e3fda5909957e29aeeb06e690495ba2d6a06c040df4d1
# gcd, x, y = gcdext(a, b)
# print('a * x - b * y =', a * x - b * y)
