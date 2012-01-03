import re
import unittest

from camellia import Camellia
from camellia import BLOCK_SIZE

class TestMontgomery(unittest.TestCase):
    def setUp(self):
        pass

    def test_simple(self):
        '''got test vector from t_camellia.txt at first'''
        k128 = bytes.fromhex('00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00')
        cm128 = Camellia(k128, 128)
        m = bytes.fromhex('80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00')
        c = bytes.fromhex('07 92 3A 39 EB 0A 81 7D 1C 4D 87 BD B8 2D 1F 1C')

        cc = cm128.encrypt(m)
        self.assertEqual(BLOCK_SIZE, len(cc))
        self.assertEqual(c, cc)

        dd = cm128.decrypt(cc)
        self.assertEqual(BLOCK_SIZE, len(dd))
        self.assertEqual(m, dd)

    def test_all_vectors(self):
        '''
        got the text vector files from
          http://info.isl.ntt.co.jp/crypt/camellia/dl/cryptrec/t_camellia.txt
        put it to
          camellia/t_camellia.txt
        '''

        f = open('camellia/t_camellia.txt')
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            if re.match(r'^$', line):
                continue
            if re.match(r'^Camellia', line):
                hit = re.search('\d\d\d', line)
                key_size = int(hit.group())
                continue
            if re.match(r'^K', line):
                hexes = line.split(':')[1]
                k = bytes.fromhex(hexes)
                cm = Camellia(k, key_size)
                continue
            if re.match(r'^P', line):
                hexes = line.split(':')[1]
                m = bytes.fromhex(hexes)
                continue
            if re.match(r'^C', line):
                hexes = line.split(':')[1]
                c = bytes.fromhex(hexes)

            cc = cm.encrypt(m)
            self.assertEqual(BLOCK_SIZE, len(cc))
            self.assertEqual(c, cc)

            dd = cm.decrypt(cc)
            self.assertEqual(BLOCK_SIZE, len(dd))
            self.assertEqual(m, dd)
        f.close()

if __name__ == '__main__':
    unittest.main()

