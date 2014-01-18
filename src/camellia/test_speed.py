# Copyright 2011 梅濁酒(umedoblock)
import datetime
import unittest

from camellia import Camellia
from camellia import BLOCK_SIZE
from camellia import _calc_size

class TestCamellia(unittest.TestCase):
    def test_cbc(self):
        print('Camellia')

        iv = b'\x00' * BLOCK_SIZE
        MB = 32
        text_size = MB * 1024 ** 2
        text = b'\00' * text_size
        for key_size in (128, 192, 256):
            print()
            print('{} bit key {} MB memory encrypt/decrypt speed'.
                format(key_size, MB))
            k = b'\x00' * (key_size // 8)
            cm = Camellia(k, key_size)

            s = datetime.datetime.now()
            cc = cm.encrypt_cbc(text, iv)
            e = datetime.datetime.now()
            encrypt_seconds =  (e - s).total_seconds()
            encrypt_speed = MB / encrypt_seconds
            print('encrypt {:.2f} seconds, speed {:.2f} MB/sec'.
                format(encrypt_seconds, encrypt_speed))

            s = datetime.datetime.now()
            dd = cm.decrypt_cbc(cc)
            e = datetime.datetime.now()
            decrypt_seconds =  (e - s).total_seconds()
            decrypt_speed = MB / decrypt_seconds
            print('decrypt {:.2f} seconds, speed {:.2f} MB/sec'.
                format(decrypt_seconds, decrypt_speed))

            # self.assertEqual(text_size, len(dd))
            # self.assertEqual(text, dd)

if __name__ == '__main__':
    unittest.main()
