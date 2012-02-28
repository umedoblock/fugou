# Copyright 2011 梅どぶろく(umedoblock)
import unittest

from ecc import *

class TestECC(unittest.TestCase):
    def test_eccp_on_different_ec(self):
        ecc0 = ECC(2, -1, 7, 11)
        ecc1 = ECC(19, 77, 307, 331)
        self.assertFalse(ecc0 == ecc1)

        eccp0 = ECCPoint(3, 2, ecc0)
        eccp1 = ECCPoint(7, 89, ecc1)
        with self.assertRaises(ECCPointError) as raiz:
            eccp0 == eccp1
        message = '''
y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7, order 11) is not
y ^ 2 = x ^ 3 + 19 * x + 77 (mod 307, order 331).'''
        args = raiz.exception.args
        self.assertEqual(message, args[0])

    def test_eccp_add_same_point(self):
        ecc = ECC(2, -1, 7, 11)
        eccp = ECCPoint(1, 3, ecc)

        double_eccp = eccp + eccp
        self.assertTrue(double_eccp.constructs(ecc))

        expected_eccp = ECCPoint(2, 2, ecc)
        self.assertEqual(expected_eccp, double_eccp)

    def test_eccp_add(self):
        ecc = ECC(2, -1, 7, 11)
        eccp0 = ECCPoint(3, 2, ecc)
        eccp1 = ECCPoint(4, 6, ecc)

        eccp01 = eccp0 + eccp1
        self.assertTrue(eccp01.constructs(ecc))

        eccp10 = eccp1 + eccp0
        self.assertTrue(eccp10.constructs(ecc))

        self.assertEqual(eccp01, eccp10)

        expected_eccp = ECCPoint(2, 2, ecc)
        self.assertEqual(expected_eccp, eccp01)
        self.assertEqual(expected_eccp, eccp10)

    def test_ecc_point_at_infinity(self):
        ecc = ECC(2, -1, 7, 11)
        eccp0 = ECCPoint(1, 3, ecc)
        eccp1 = ECCPoint(1, 4, ecc)

        added_eccp = eccp0 + eccp1
        self.assertTrue(added_eccp.isinf())
        self.assertEqual('(inf, inf)', str(added_eccp))

        with self.assertRaises(ECCPointError) as raiz:
            ECCPoint(0, 0, ecc)
        args = raiz.exception.args
        message = \
            '(0, 0) is not on y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7, order 11).'
        self.assertEqual(message, args[0])

        point_at_infinity = ECCPoint(0, 0, ecc, is_infinity=True)

        eccp2 = eccp0 + point_at_infinity
        self.assertEqual(eccp0, eccp2)

        eccp3 = point_at_infinity + eccp1
        self.assertEqual(eccp1, eccp3)

        point_at_infinity2 = ECCPoint(0, 0, ecc, is_infinity=True)
        added_infinity = point_at_infinity + point_at_infinity2
        self.assertTrue(added_infinity.isinf())
        self.assertEqual(point_at_infinity2, added_infinity)

    def test_calc_all_pair_of_xy(self):
        ecc = ECC(2, -1, 7, 11)
        points = ecc.collect_all_points()
        self.assertEqual(11, len(points))
        expected_points = frozenset([
            ECCPoint(0, 0, ecc, is_infinity=True),
            ECCPoint(1, 3, ecc),
            ECCPoint(1, 4, ecc),
            ECCPoint(2, 2, ecc),
            ECCPoint(2, 5, ecc),
            ECCPoint(3, 2, ecc),
            ECCPoint(3, 5, ecc),
            ECCPoint(4, 1, ecc),
            ECCPoint(4, 6, ecc),
            ECCPoint(5, 1, ecc),
            ECCPoint(5, 6, ecc),
        ])
        self.assertEqual(expected_points, points)

    def test_calc_pair_of_xy(self):
        ecc = ECC(2, -1, 7, 11)

        x, y = ecc.calc_pair_of_xy(x=2)
        # print('x, y =', x, y)
        self.assertEqual((2, 2), (x, y))

        x, y = ecc.calc_pair_of_xy(x=5)
        # print('x, y =', x, y)
        self.assertEqual((5, 1), (x, y))

        x, y = ecc.calc_pair_of_xy(y=4)
        self.assertEqual((1, 4), (x, y))

        x, y = ecc.calc_pair_of_xy(y=6)
        self.assertEqual((4, 6), (x, y))

        p = ecc.calc_pair_of_xy(y=0)
        self.assertFalse(p)

    def test_ecc(self):
        ecc = ECC(2, -1, 7, 11)
        self.assertEqual(2, ecc.a)
        self.assertEqual(-1, ecc.b)
        self.assertEqual(7, ecc.prime)
        self.assertEqual(11, ecc.order)
        self.assertEqual('y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7, order 11)', \
                         str(ecc))

    def test_ecc_and_points(self):
        ecc = ECC(2, -1, 7, 11)
        p0 = Point(1, 3)
        p1 = Point(5, 6)

        self.assertTrue(ecc.exists_with(p0))
        self.assertTrue(ecc.exists_with(p1))

        self.assertTrue(p0.constructs(ecc))
        self.assertTrue(p1.constructs(ecc))
        self.assertTrue(p0.is_on(ecc))
        self.assertTrue(p1.is_on(ecc))

        eccp = ECCPoint(4, 1, ecc)
        self.assertTrue(eccp)

    def test_ecc_and_points_not_relate(self):
        ecc = ECC(2, -1, 7, 11)
        p0 = Point(0, 0)

        self.assertFalse(ecc.exists_with(p0))
        self.assertFalse(p0.constructs(ecc))
        self.assertFalse(p0.is_on(ecc))

        with self.assertRaises(ECCPointError) as raiz:
            ECCPoint(0, 0, ecc)
        args = raiz.exception.args
        message = \
            '(0, 0) is not on y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7, order 11).'
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
