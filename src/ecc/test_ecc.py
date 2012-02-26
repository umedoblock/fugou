# Copyright 2011 梅どぶろく(umedoblock)
import unittest

from ecc import gcdext

class TestECC(unittest.TestCase):
    def test_gcdext(self):
        a, b = 5, 7
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        self.assertEqual(3, x)
        self.assertEqual(2, y)
        self.assertEqual(1, a * x - b * y)

        a, b = 7, 5
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        self.assertEqual(3, x)
        self.assertEqual(4, y)
        self.assertEqual(1, a * x - b * y)

        a, b = 1007, 107
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        self.assertEqual(90, x)
        self.assertEqual(847, y)
        self.assertEqual(1, a * x - b * y)

        a, b = 107, 1007
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        self.assertEqual(160, x)
        self.assertEqual(17, y)
        self.assertEqual(1, a * x - b * y)

        a = 0xf85744946adc388cb788e3fda5909957e29aeeb06e690495ba2d6a06c040df4d1
        b = 0x3008c0921487ab2efa77950977e5c8a7c4fcd70909296ca07b521dd91213153ab
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        expected_x = \
            0x2b201a984d2e6d5c88a71cd3c1b809d9ceab4f604022e649fde7cb35336a37247
        expected_y = \
            0xdef649a9f15abf7b8843843193fb38b8cca5bc0b70cf75f380839c6605cd651e2
        self.assertEqual(expected_x, x)
        self.assertEqual(expected_y, y)
        self.assertEqual(1, a * x - b * y)

        a = 0x3008c0921487ab2efa77950977e5c8a7c4fcd70909296ca07b521dd91213153ab
        b = 0xf85744946adc388cb788e3fda5909957e29aeeb06e690495ba2d6a06c040df4d1
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, gcd)
        expected_x = \
            0x1960faea798179112f455fcc1195609f15f532a4fd998ea239a9cda0ba737a2ef
        expected_y = \
            0x4e8a5f9c7593dd271d07835b62dbecdf65187a8c90686567d6a52a3dea8de164
        self.assertEqual(expected_x, x)
        self.assertEqual(expected_y, y)
        self.assertEqual(1, a * x - b * y)

if __name__ == '__main__':
    unittest.main()
