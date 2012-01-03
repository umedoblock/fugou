import unittest

from camellia import Camellia
from camellia import BLOCK_SIZE

class TestMontgomery(unittest.TestCase):
    def setUp(self):
        pass

    def test__init__(self):
        '''
        got the text vector files from
          http://info.isl.ntt.co.jp/crypt/camellia/dl/cryptrec/t_camellia.txt
          http://info.isl.ntt.co.jp/crypt/camellia/dl/cryptrec/intermediate.txt
        put it to
          camellia/t_camellia.txt
          camellia/intermediate.txt
        '''
        k128 = bytes.fromhex('00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00')
        k192 = bytes.fromhex(''' 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00''')
        k256 = bytes.fromhex(''' 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00''')
        print('k128 = ', type(k128), k128)
        cm128 = Camellia(k128, 128)
        m = bytes.fromhex('80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00')
        c = bytes.fromhex('07 92 3A 39 EB 0A 81 7D 1C 4D 87 BD B8 2D 1F 1C')
        print('cm128 =', cm128)

        cc = cm128.encrypt(m)
        self.assertEqual(BLOCK_SIZE, len(cc))
        self.assertEqual(c, cc)

        dd = cm128.decrypt(cc)
        self.assertEqual(BLOCK_SIZE, len(dd))
        self.assertEqual(m, dd)

if __name__ == '__main__':
    unittest.main()

