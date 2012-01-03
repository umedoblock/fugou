import unittest

from camellia import Camellia

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
        print(cm128)

if __name__ == '__main__':
    unittest.main()

