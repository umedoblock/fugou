import re
import unittest

from camellia import Camellia
from camellia import BLOCK_SIZE

class TestMontgomery(unittest.TestCase):
    def setUp(self):
        pass

    def test_cbc(self):
        text_sizes = (BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE + 1, 100)
        for key_size in (128, 192, 256):
            print('key_size =', key_size)
            k = b'\x00' * (key_size // 8)
            cm = Camellia(k, key_size)
            for text_size in text_sizes:
                print('text_size =', text_size)
                m = b'\x83' * text_size
                cc = cm.encrypt_cbc(m)
                # print('cc =', cc)
                # self.assertEqual(BLOCK_SIZE, len(cc))
                # self.assertEqual(c, cc)

                # dd = cm.decrypt_cbc(cc)
                # self.assertEqual(text_size, len(dd))
                # self.assertEqual(m, dd)
            print()

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

    def test_exeption(self):
        k = b'\00' * (128 // 8)
        cm = Camellia(k, 128)

        for invalid_keysize in (127, 129, 191, 193, 255, 257, 0, -1):
            with self.assertRaises(ValueError) as raiz:
                Camellia(k, invalid_keysize)
            args = raiz.exception.args
            self.assertEqual(args[0], 'key_size must be 128, 192 or 256.')

        with self.assertRaises(ValueError) as raiz:
            invalid_length_k = b'\00' * 15
            Camellia(invalid_length_k, 128)
        args = raiz.exception.args
        self.assertEqual(args[0], 'len(key) * 8 must be longer than key_size.')

        s = str(k)
        with self.assertRaises(TypeError) as raiz:
            Camellia(s, 128)
        args = raiz.exception.args
        self.assertEqual(args[0], 'type(key) must be bytes.')

        m = b'\00' * (BLOCK_SIZE - 1)
        with self.assertRaises(ValueError) as raiz:
            cm.encrypt(m)
        args = raiz.exception.args
        expected_message = 'text length must be longer than 16 octets.'
        self.assertEqual(expected_message, args[0])

        c = b'\00' * (BLOCK_SIZE - 1)
        with self.assertRaises(ValueError) as raiz:
            cm.decrypt(c)
        args = raiz.exception.args
        expected_message = 'cipher length must be longer than 16 octets.'
        self.assertEqual(expected_message, args[0])

    def test_no_exeption(self):
        k = b'\00' * ((128 // 8) + 1)
        cm = Camellia(k, 128)

        m = b'\00' * (BLOCK_SIZE + 1)
        cm.encrypt(m)

        c = b'\00' * (BLOCK_SIZE + 1)
        cm.decrypt(c)

if __name__ == '__main__':
    unittest.main()
