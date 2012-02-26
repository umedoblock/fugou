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
        self.assertEqual(1, a * x - b * y)
        self.assertEqual(1, gcd)
        self.assertEqual(90, x)
        self.assertEqual(847, y)
        self.assertEqual(1, a * x - b * y)

        a, b = 107, 1007
        gcd, x, y = gcdext(a, b)
        self.assertEqual(1, a * x - b * y)
        self.assertEqual(1, gcd)
        self.assertEqual(160, x)
        self.assertEqual(17, y)
        self.assertEqual(1, a * x - b * y)

if __name__ == '__main__':
    unittest.main()
