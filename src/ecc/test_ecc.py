# Copyright 2011 梅どぶろく(umedoblock)
import unittest

from ecc import *
from ecc import Point
from ecc import gcdext
from ecdh import *

class TestECDH(unittest.TestCase):
    def test_ecdh_256bit_random_ECP_Group(self):
        # test data from http://www.rfc-editor.org/rfc/rfc5903.txt
        # make ecc.
        a = -3
        b = int.from_bytes(bytes.fromhex(
                 '5AC635D8 AA3A93E7 B3EBBD55 769886BC'
                 '651D06B0 CC53B0F6 3BCE3C3E 27D2604B'), 'big')
        prime = int.from_bytes(bytes.fromhex(
                    'FFFFFFFF 00000001 00000000 00000000'
                    '00000000 FFFFFFFF FFFFFFFF FFFFFFFF'), 'big')
        order = int.from_bytes(bytes.fromhex(
                    'FFFFFFFF 00000000 FFFFFFFF FFFFFFFF'
                    'BCE6FAAD A7179E84 F3B9CAC2 FC632551'), 'big')
        ecc256 = ECC(a, b, prime, order)

        # make generator.
        gx = int.from_bytes(bytes.fromhex(
                    '6B17D1F2 E12C4247 F8BCE6E5 63A440F2'
                    '77037D81 2DEB33A0 F4A13945 D898C296'), 'big')
        gy = int.from_bytes(bytes.fromhex(
                    '4FE342E2 FE1A7F9B 8EE7EB4A 7C0F9E16'
                    '2BCE3357 6B315ECE CBB64068 37BF51F5'), 'big')
        generator256 = ECCPoint(gx, gy, ecc256)
        alice = ECDH(generator256)
        bob = ECDH(generator256)

        # make alice's public key.
        i = int.from_bytes(bytes.fromhex(
                    'C88F01F5 10D9AC3F 70A292DA A2316DE5'
                    '44E9AAB8 AFE84049 C62A9C57 862D1433'), 'big')
        alice.set_private_key(private_key=i)
        alice.compute_public_key()
        # check alice's public key.
        gix = int.from_bytes(bytes.fromhex(
                    'DAD0B653 94221CF9 B051E1FE CA5787D0'
                    '98DFE637 FC90B9EF 945D0C37 72581180'), 'big')
        giy = int.from_bytes(bytes.fromhex(
                    '5271A046 1CDB8252 D61F1C45 6FA3E59A'
                    'B1F45B33 ACCF5F58 389E0577 B8990BB3'), 'big')
        gi = ECCPoint(gix, giy, ecc256)
        self.assertEqual(gi, alice.get_public_key())

        # make bob's public key.
        r = int.from_bytes(bytes.fromhex(
                    'C6EF9C5D 78AE012A 011164AC B397CE20'
                    '88685D8F 06BF9BE0 B283AB46 476BEE53'), 'big')
        bob.set_private_key(private_key=r)
        bob.compute_public_key()
        # check bob's public key.
        grx = int.from_bytes(bytes.fromhex(
                    'D12DFB52 89C8D4F8 1208B702 70398C34'
                    '2296970A 0BCCB74C 736FC755 4494BF63'), 'big')
        gry = int.from_bytes(bytes.fromhex(
                    '56FBF3CA 366CC23E 8157854C 13C58D6A'
                    'AC23F046 ADA30F83 53E74F33 039872AB'), 'big')
        gr = ECCPoint(grx, gry, ecc256)
        self.assertEqual(gr, bob.get_public_key())

        self.assertNotEqual(alice.get_private_key(), bob.get_private_key())
        self.assertNotEqual(alice.get_public_key(), bob.get_public_key())

        # alice's turn.
        alice.make_secret_key(bob.get_public_key())
        alice_secret_key = alice.get_secret_key()

        # bob's turn.
        bob.make_secret_key(alice.get_public_key())
        bob_secret_key = bob.get_secret_key()

        # alice and bob finished ECDH protocol.

        # make expected secret key
        girx = int.from_bytes(bytes.fromhex(
                    'D6840F6B 42F6EDAF D13116E0 E1256520'
                    '2FEF8E9E CE7DCE03 812464D0 4B9442DE'), 'big')
        giry = int.from_bytes(bytes.fromhex(
                    '522BDE0A F0D8585B 8DEF9C18 3B5AE38F'
                    '50235206 A8674ECB 5D98EDB2 0EB153A2'), 'big')
        gir = ECCPoint(girx, giry, ecc256)

        # check
        self.assertEqual(gir, alice_secret_key)
        self.assertEqual(gir, bob_secret_key)

    def test_ecdh_basic(self):
        ecc = ECC(19, 77, 307, 331)
        generator = ECCPoint(7, 218, ecc)
        alice = ECDH(generator)
        bob = ECDH(generator)

        alice.set_private_key()
        bob.set_private_key()
        self.assertNotEqual(alice.get_private_key(), bob.get_private_key())

        alice.compute_public_key()
        bob.compute_public_key()
        self.assertNotEqual(alice.get_public_key(), bob.get_public_key())

        alice_secret_key = alice.make_secret_key(bob.get_public_key())
        bob_secret_key = bob.make_secret_key(alice.get_public_key())

        self.assertEqual(alice_secret_key, bob_secret_key)

class TestECC(unittest.TestCase):

    def test_simple_ecdh(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)
        P178 = ECCPoint(162, 225, ecc)
        P217 = ECCPoint(199, 151, ecc)
        alice_key = 75
        bob_key = 234
        muled_key = alice_key * bob_key % ecc.order

        PA = alice_key * P8
        PB = bob_key * P8

        self.assertEqual(P26, PA)
        self.assertEqual(P178, PB)

        AlicePoint = alice_key * PB
        BobPoint = bob_key * PA

        self.assertEqual(P217, AlicePoint)
        self.assertEqual(P217, BobPoint)
        self.assertEqual(AlicePoint, BobPoint)

        muled_point = muled_key * P8
        self.assertEqual(P217, muled_point)

    def test_eccp_div(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        num = 111

        Q = num * P8
        self.assertNotEqual(P8, Q)

        P8_ = Q / num
        self.assertEqual(P8, P8_)

        with self.assertRaises(TypeError) as raiz:
            P8_ = Q // num
        args = raiz.exception.args
        self.assertEqual(args[0],
                    ("unsupported operand type(s) for //: "
                     "'ECCPoint' and 'int'"))

    def test_eccp_mul_by_negative_value(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)

        with self.assertRaises(ValueError) as raiz:
            P8 * -1
        args = raiz.exception.args
        self.assertEqual('number(=-1) must be positive value.', args[0])

    def test_eccp_mul(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        x = P8 * 75
        self.assertEqual(P26, x)

        y = 75 * P8
        self.assertEqual(P26, y)

        z = P8
        z *= 75
        self.assertEqual(P26, z)

    def test_eccp_mul_fast(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        P8_75 = ecc.mul_fast(P8, 75)
        self.assertEqual(P26, P8_75)

    def test_eccp_mul_honest(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        P8_75 = ecc.mul_honest(P8, 75)
        self.assertEqual(P26, P8_75)

    def test_eccp_on_different_ec(self):
        ecc0 = ECC(2, -1, 7, 11)
        ecc1 = ECC(19, 77, 307, 331)
        self.assertFalse(ecc0 == ecc1)

        eccp0 = ECCPoint(3, 2, ecc0)
        eccp1 = ECCPoint(7, 89, ecc1)
        with self.assertRaises(ECCPointError) as raiz:
            eccp0 == eccp1

        message = '''
y ^ 2 = x ^ 3 + 2 * x - 1 (mod 7, order 11) and
y ^ 2 = x ^ 3 + 19 * x + 77 (mod 307, order 331)
are different ECC.
Therefore, __eq__() cannot compare (3, 2) with (7, 89).'''
        args = raiz.exception.args
        self.assertEqual(message, args[0])

    def test_eccp_add_same_point(self):
        ecc = ECC(2, -1, 7, 11)
        eccp = ECCPoint(1, 3, ecc)
        expected_eccp = ECCPoint(2, 2, ecc)

        double_eccp = eccp + eccp
        self.assertTrue(double_eccp.constructs(ecc))
        self.assertEqual(expected_eccp, double_eccp)

        double_eccp_ = eccp
        double_eccp_ += eccp
        self.assertEqual(expected_eccp, double_eccp_)
        self.assertTrue(double_eccp_.constructs(ecc))

    def test_eccp_add(self):
        ecc = ECC(2, -1, 7, 11)
        eccp0 = ECCPoint(3, 2, ecc)
        eccp1 = ECCPoint(4, 6, ecc)
        expected_eccp = ECCPoint(2, 2, ecc)

        x = eccp0 + eccp1
        self.assertTrue(x.constructs(ecc))
        self.assertEqual(expected_eccp, x)

        y = eccp1 + eccp0
        self.assertTrue(y.constructs(ecc))
        self.assertEqual(expected_eccp, y)

        self.assertEqual(x, y)

    def test_eccp_bool(self):
        ecc = ECC(2, -1, 7)
        P = ECCPoint(3, 2, ecc)
        self.assertTrue(P)
        Q = ECCPoint(0, 0, ecc, is_infinity=True)
        self.assertFalse(Q)

    def test_ecc_the_group_low(self):
        # http://en.wikipedia.org/wiki/Elliptic_curve
        # The group law
        ecc = ECC(2, -1, 7, 11)
        point_at_infinity = ECCPoint(0, 0, ecc, is_infinity=True)

        # case 1.
        P = ECCPoint(1, 3, ecc)
        Q = ECCPoint(4, 1, ecc)
        R_ = P + Q
        R = ECCPoint(R_.x, -R_.y, ecc)
        self.assertEqual(point_at_infinity, P + Q + R)
        P = Q = R = P_ = R_ = None

        # case 2.
        Q = ECCPoint(1, 3, ecc)
        P_ = Q + Q
        P = ECCPoint(P_.x, -P_.y, ecc)
        self.assertEqual(point_at_infinity, P + Q + Q)
        P = Q = R = P_ = R_ = None

        # case 3.
        P = ECCPoint(1, 3, ecc)
        Q = ECCPoint(P.x, -P.y, ecc)
        self.assertEqual(point_at_infinity, P + Q + point_at_infinity)
        P = Q = R = P_ = R_ = None

        # case 4.
        # D.N.E in ECC.
        # no need to think about Elliptic Curve.
        pass

    def test_ecc_point_at_infinity_with_different_ecc(self):
        ecc0 = ECC(2, -1, 7, 11)
        ecc1 = ECC(19, 77, 307)

        point_at_infinity0 = ECCPoint(0, 0, ecc0, is_infinity=True)

        self.assertTrue(ecc0.exists_with(point_at_infinity0))

        self.assertFalse(ecc1.exists_with(point_at_infinity0,
                                          raise_error=False))
        with self.assertRaises(ECCPointError) as raiz:
            ecc1.exists_with(point_at_infinity0, raise_error=True)

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

        self.assertTrue(ecc.exists_with(point_at_infinity))

        eccp2 = eccp0 + point_at_infinity
        self.assertEqual(eccp0, eccp2)

        eccp3 = point_at_infinity + eccp1
        self.assertEqual(eccp1, eccp3)

        point_at_infinity2 = ECCPoint(0, 0, ecc, is_infinity=True)
        added_infinity = point_at_infinity + point_at_infinity2
        self.assertTrue(added_infinity.isinf())
        self.assertEqual(point_at_infinity2, added_infinity)

    def test_calc_order(self):
        ecc = ECC(19, 77, 307)
        self.assertEqual(0, ecc.order)

        order = ecc.calc_order()

        self.assertEqual(331, order)
        self.assertEqual(331, ecc.order)
      # print(ecc.collect_all_points())

    def test_calc_all_pair_of_xy(self):
        ecc = ECC(2, -1, 7)
        self.assertEqual(0, ecc.order)

        points = ecc.collect_all_points()

        self.assertFalse(0, ecc.order)
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

        self.assertFalse(ecc.exists_with(p0, raise_error=False))
        self.assertFalse(p0.constructs(ecc, raise_error=False))
        self.assertFalse(p0.is_on(ecc, raise_error=False))

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
