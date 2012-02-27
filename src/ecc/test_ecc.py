# Copyright 2011 梅どぶろく(umedoblock)
import unittest

from ecc import *

class TestECC(unittest.TestCase):
    def test_ecp_on_different_ec(self):
        ec0 = EC(2, -1, 7, 11)
        ec1 = EC(19, 77, 307, 331)
        ecp0 = ECPoint(3, 2, ec0)
        ecp1 = ECPoint(7, 89, ec1)

        self.assertFalse(ec0 == ec1)
        with self.assertRaises(ECPointError) as raiz:
            ecp0 == ecp1
        message = '''
y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7) is not
y ^ 2 = x ^ 3 + 19 * x + 77 (mod 307).'''
        args = raiz.exception.args
        self.assertEqual(message, args[0])

    def test_ecp_add_same_point(self):
        ec = EC(2, -1, 7, 11)
        ecp = ECPoint(1, 3, ec)

        double_ecp = ecp + ecp
        self.assertTrue(double_ecp.constructs(ec))

        expected_ecp = ECPoint(2, 2, ec)
        self.assertEqual(expected_ecp, double_ecp)

    def test_ecp_add(self):
        ec = EC(2, -1, 7, 11)
        ecp0 = ECPoint(3, 2, ec)
        ecp1 = ECPoint(4, 6, ec)

        ecp01 = ecp0 + ecp1
        self.assertTrue(ecp01.constructs(ec))

        ecp10 = ecp1 + ecp0
        self.assertTrue(ecp10.constructs(ec))

        self.assertEqual(ecp01, ecp10)

        expected_ecp = ECPoint(2, 2, ec)
        self.assertEqual(expected_ecp, ecp01)
        self.assertEqual(expected_ecp, ecp10)

    def test_ec(self):
        ec = EC(2, -1, 7, 11)
        self.assertEqual(2, ec.a)
        self.assertEqual(-1, ec.b)
        self.assertEqual(7, ec.prime)
        self.assertEqual(11, ec.order)

    def test_ec_and_points(self):
        ec = EC(2, -1, 7, 11)
        p0 = Point(1, 3)
        p1 = Point(5, 6)

        self.assertTrue(ec.exists_with(p0))
        self.assertTrue(ec.exists_with(p1))

        self.assertTrue(p0.constructs(ec))
        self.assertTrue(p1.constructs(ec))
        self.assertTrue(p0.is_on(ec))
        self.assertTrue(p1.is_on(ec))

        ecp = ECPoint(4, 1, ec)
        self.assertTrue(ecp)

    def test_ec_and_points_not_relate(self):
        ec = EC(2, -1, 7, 11)
        p0 = Point(0, 0)

        self.assertFalse(ec.exists_with(p0))
        self.assertFalse(p0.constructs(ec))
        self.assertFalse(p0.is_on(ec))

        with self.assertRaises(ECPointError) as raiz:
            ECPoint(0, 0, ec)
        args = raiz.exception.args
        message = '(0, 0) is not on y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7).'
        self.assertEqual(message, args[0])

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
