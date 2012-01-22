import re
import unittest

from camellia import Camellia
from camellia import BLOCK_SIZE
from camellia import _calc_size

class TestPorker(unittest.TestCase):
    def test_porker(self):
        key_size = 128
        k1 = b'\x01' * (key_size // 8)
        k2 = b'\x02' * (key_size // 8)

        m = b'\x00' * BLOCK_SIZE

        cm1 = Camellia(k1, key_size)
        cm2 = Camellia(k2, key_size)

        c1 = cm1.encrypt(m)
        c2 = cm2.encrypt(m)
        print('c1 == c2 is', c1 == c2)
        print('c1 =', c1)
        print('c2 =', c2)

        c21 = cm1.encrypt(c2)
        c12 = cm2.encrypt(c1)

        print('c21 == c12 is', c12 == c21)
        print('c21 =', c21)
        print('c12 =', c12)

if __name__ == '__main__':
    unittest.main()
