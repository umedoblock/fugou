# author: 梅濁酒(umedoblock)
# references: 妖精現実フェアリアル http://deztecc.jp/x/05/faireal/23-index.html
#           : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

import unittest

from ecc import *
from ecc import Point
from ecdh import *
from ecdsa import *

class TestECDSA(unittest.TestCase):
    def test_ecdsa_256bit_random(self):
        # test data from http://www.rfc-editor.org/rfc/rfc4754.txt
        expected_w = int.from_bytes(bytes.fromhex(
                    'DC51D386 6A15BACD E33D96F9 92FCA99D'
                    'A7E6EF09 34E70975 59C27F16 14C88A7F'), 'big')

        expected_r = int.from_bytes(bytes.fromhex(
                    'CB28E099 9B9C7715 FD0A80D8 E47A7707'
                    '9716CBBF 917DD72E 97566EA1 C066957C'), 'big')

        expected_s = int.from_bytes(bytes.fromhex(
                    '86FA3BB4 E26CAD5B F90B7F81 899256CE'
                    '7594BB1E A0C89212 748BFF3B 3D5B0315'), 'big')

        ecc256, G256 = ECCGetItem(256)

        # signer: alice
        # verifier: bob
        # alice doesn't need to calculate public key for signature.
        # alice needs just to set private key.
        alice = ECDSA(G256)
        bob = ECDSA(G256)

        with self.assertRaises(ValueError) as raiz:
            alice.get_private_key()
        args = raiz.exception.args
        self.assertEqual(args[0], '_private_key is empty value.')

        alice.set_private_key(private_key=expected_w)
        pri = alice.get_private_key()
        self.assertEqual(expected_w, pri)

        # stand in randrange for this test.
        # alice recieve k as random number in sign().
        k = int.from_bytes(bytes.fromhex(
                    '9E56F509 196784D9 63D1C0A4 01510EE7'
                    'ADA3DCC5 DEE04B15 4BF61AF1 D5A6DECE'), 'big')
        def randrange_stand_in(_min, _max, v=k):
            return v
        import ecc
        d = ecc.__dict__['generate_random'].__dict__
        d['randrange'] = randrange_stand_in

        h = int.from_bytes(bytes.fromhex(
                    'BA7816BF 8F01CFEA 414140DE 5DAE2223'
                    'B00361A3 96177A9C B410FF61 F20015AD'), 'big')
        # alice doesn't need calculated public key for signature.
        # alice needs just to set private key.
        r, s = alice.sign(h)
        self.assertEqual(k, generate_random(ecc256.order))
        del d['randrange']
        self.assertNotEqual(k, generate_random(ecc256.order))
        self.assertEqual(expected_r, r)
        self.assertEqual(expected_s, s)

        gwx = int.from_bytes(bytes.fromhex(
                    '2442A5CC 0ECD015F A3CA31DC 8E2BBC70'
                    'BF42D60C BCA20085 E0822CB0 4235E970'), 'big')
        gwy = int.from_bytes(bytes.fromhex(
                    '6FC98BD7 E50211A4 A27102FA 3549DF79'
                    'EBCB4BF2 46B80945 CDDFE7D5 09BBFD7D'), 'big')
        alice_public_key = ECCPoint(gwx, gwy, ecc256)
        result = bob.verify(r, s, h, alice_public_key)
        self.assertTrue(result)

class TestECDH(unittest.TestCase):
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

        Alice_compute = alice_key * PB
        Bob_compute = bob_key * PA

        self.assertEqual(P217, Alice_compute)
        self.assertEqual(P217, Bob_compute)
        self.assertEqual(Alice_compute, Bob_compute)

        muled_point = muled_key * P8
        self.assertEqual(P217, muled_point)

    def test_ecdh_basic(self):
        ecc = ECC(19, 77, 307, 331)
        G = ECCPoint(7, 218, ecc)
        alice = ECDH(G)
        bob = ECDH(G)

        alice.set_private_key()
        bob.set_private_key()
        self.assertNotEqual(alice.get_private_key(), bob.get_private_key())

        alice.compute_public_key()
        bob.compute_public_key()
        self.assertNotEqual(alice.get_public_key(), bob.get_public_key())

        alice_secret_key = alice.make_secret_key(bob.get_public_key())
        bob_secret_key = bob.make_secret_key(alice.get_public_key())

        self.assertEqual(alice_secret_key, bob_secret_key)

    def test_ecdh_256bit_random_ECP_Group(self):
        ecc256, G256 = ECCGetItem(256)
        alice = ECDH(G256)
        bob = ECDH(G256)

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

class TestECC(unittest.TestCase):

    # essential test ##########################################################
    def test_ecc(self):
        ecc = ECC(2, -1, 7, 11)
        self.assertEqual(2, ecc.a)
        self.assertEqual(-1, ecc.b)
        self.assertEqual(7, ecc.prime)
        self.assertEqual(11, ecc.order)
        self.assertEqual(
                'y ^ 2 = x ^ 3 + 0x2 * x - 0x1 (mod 0x7, order 0xb)',
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

        P = ECCPoint(4, 1, ecc)
        self.assertTrue(P)

    def test_calc_order(self):
        ecc = ECC(19, 77, 307)
        self.assertEqual(0, ecc.order)

        order = ecc.calc_order()

        self.assertEqual(331, order)
        self.assertEqual(331, ecc.order)
      # print(ecc.collect_all_points())

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
            ('(0x0, 0x0) is not on y ^ 2 = x ^ 3 + 0x2 * x - 0x1 '
             '(mod 0x7, order 0xb).')
        self.assertEqual(message, args[0])

    def test_ECCPoint_bool(self):
        ecc = ECC(2, -1, 7)
        P = ECCPoint(3, 2, ecc)
        self.assertTrue(P)
        Q = ECCPoint(0, 0, ecc, is_infinity=True)
        self.assertFalse(Q)

    def test_ECCPoint_eq_ne(self):
        ecc = ECC(19, 77, 307, 331)
        # How to find a point on ecc ?
        # Please run and see ecc/generate_points_on_ec.py
        P = ECCPoint(0xd, 0x28, ecc)
        P_ = ECCPoint(0xd, 0x28, ecc)
        Q = ECCPoint(0x3b, 0xcd, ecc)

        self.assertEqual(P, P_)
        self.assertNotEqual(P, Q)

    def test_ECCPoint_gt_lt(self):
        ecc = ECC(19, 77, 307, 331)
        # same x
        P = ECCPoint(0xd, 0x28, ecc)
        Q = ECCPoint(0xd, 0x10b, ecc)
        # same y
        R = ECCPoint(0x4f, 0x1f, ecc)
        S = ECCPoint(0x61, 0x1f, ecc)

        # same x
        self.assertLess(P, Q)
        self.assertGreater(Q, P)

        # same y
        self.assertLess(R, S)
        self.assertGreater(S, R)

        # smallest and biggest
        self.assertLess(P, S)
        self.assertGreater(S, P)

        # for a rainy day
        self.assertLess(Q, R)
        self.assertGreater(R, Q)

    def test_ECCPoint_ge_le(self):
        ecc = ECC(19, 77, 307, 331)
        P = ECCPoint(0xd, 0x28, ecc)
        P_ = ECCPoint(0xd, 0x28, ecc)

        self.assertGreaterEqual(P, P_)
        self.assertLessEqual(P, P_)

    def test_ecc_point_at_infinity(self):
        # point at infinity names O.
        ecc = ECC(2, -1, 7, 11)

        O = ECCPoint(0, 0, ecc, is_infinity=True)
        self.assertTrue(ecc.exists_with(O))
        self.assertTrue(O.isinf())
        self.assertEqual('(inf, inf)', str(O))

        with self.assertRaises(ECCPointError) as raiz:
            ECCPoint(0, 0, ecc)
        args = raiz.exception.args
        message = \
            ('(0x0, 0x0) is not on y ^ 2 = x ^ 3 + 0x2 * x - 0x1 '
             '(mod 0x7, order 0xb).')
        self.assertEqual(message, args[0])

        P = ECCPoint(1, 3, ecc)

        PO = P + O
        self.assertEqual(P, PO)
        OP = O + P
        self.assertEqual(P, OP)
        P0 = 0 * P
        self.assertEqual(O, P0)

        O_ = ECCPoint(0, 0, ecc, is_infinity=True)
        OO_ = O + O_
        self.assertTrue(OO_.isinf())
        self.assertEqual(O, OO_)
        self.assertEqual(O_, OO_)

        # P + Q = O
        Q = ECCPoint(1, 4, ecc)
        R = P + Q
        self.assertEqual(O, R)
        self.assertTrue(R.isinf())
        self.assertEqual('(inf, inf)', str(R))

    def test_ECCPoint_compare_with_infinity(self):
        ecc = ECC(19, 77, 307, 331)
        P = ECCPoint(0x12e, 0xde, ecc)
        O = ECCPoint(0, 0, ecc, is_infinity=True)

        self.assertFalse(P.isinf())
        self.assertTrue(O.isinf())

        self.assertLess(P, O)
        self.assertLessEqual(P, O)
        self.assertGreater(O, P)
        self.assertGreaterEqual(O, P)

        self.assertNotEqual(P, O)

    def test_ECCPoint_compare_with_infinity_infinity(self):
        ecc = ECC(19, 77, 307, 331)
        O = ECCPoint(0, 0, ecc, is_infinity=True)
        O2 = ECCPoint(0, 0, ecc, is_infinity=True)

        self.assertTrue(O.isinf())
        self.assertTrue(O2.isinf())

        self.assertEqual(O, O2)

        self.assertLessEqual(O, O2)
        self.assertGreaterEqual(O, O2)

        self.assertFalse(O > O2)
        self.assertFalse(O < O2)

    def test_ECCPoint_on_different_ec(self):
        ecc_0 = ECC(2, -1, 7, 11)
        ecc_1 = ECC(19, 77, 307, 331)
        self.assertFalse(ecc_0 == ecc_1)

        P = ECCPoint(3, 2, ecc_0)
        Q = ECCPoint(7, 89, ecc_1)
        with self.assertRaises(ECCPointError) as raiz:
            P == Q

        message = '''
y ^ 2 = x ^ 3 + 0x2 * x - 0x1 (mod 0x7, order 0xb) and
y ^ 2 = x ^ 3 + 0x13 * x + 0x4d (mod 0x133, order 0x14b)
are different ECC.
Therefore, __eq__() cannot compare (0x3, 0x2) with (0x7, 0x59).'''
        args = raiz.exception.args
        self.assertEqual(message, args[0])

    def test_ecc_point_at_infinity_with_different_ecc(self):
        ecc_0 = ECC(2, -1, 7, 11)
        ecc_1 = ECC(19, 77, 307)

        O_ecc_0 = ECCPoint(0, 0, ecc_0, is_infinity=True)

        self.assertTrue(ecc_0.exists_with(O_ecc_0))

        self.assertFalse(ecc_1.exists_with(O_ecc_0,
                                          raise_error=False))
        with self.assertRaises(ECCPointError) as raiz:
            ecc_1.exists_with(O_ecc_0)
        with self.assertRaises(ECCPointError) as raiz:
            ecc_1.exists_with(O_ecc_0, raise_error=True)

    # basic test ##############################################################
    def test_ECCPoint_add(self):
        ecc = ECC(2, -1, 7, 11)
        P = ECCPoint(3, 2, ecc)
        Q = ECCPoint(4, 6, ecc)
        expected_PQ = ECCPoint(2, 2, ecc)

        PQ = P + Q
        self.assertTrue(PQ.constructs(ecc))
        self.assertEqual(expected_PQ, PQ)

        QP = Q + P
        self.assertTrue(PQ.constructs(ecc))
        self.assertEqual(expected_PQ, QP)

        self.assertEqual(PQ, QP)

    def test_ECCPoint_add_same_point(self):
        ecc = ECC(2, -1, 7, 11)
        P = ECCPoint(1, 3, ecc)
        expected_P2 = ECCPoint(2, 2, ecc)

        P2_add = P + P
        self.assertTrue(P2_add.constructs(ecc))
        self.assertEqual(expected_P2, P2_add)

        P2_iadd = P
        P2_iadd += P
        self.assertEqual(expected_P2, P2_iadd)
        self.assertTrue(P2_iadd.constructs(ecc))

        P2_self = P
        P2_self += P2_self
        self.assertEqual(expected_P2, P2_self)
        self.assertTrue(P2_self.constructs(ecc))

        self.assertEqual(ECCPoint(1, 3, ecc), P)

    def test_same_y_axis(self):
        # 気になったので。
        ecc = ECC(19, 77, 307, 331)
        P = ECCPoint(0xaa, 0x3, ecc)
        Q = ECCPoint(0xbe, 0x3, ecc)

        R = P + Q
      # print("R =")
      # print(R)
        self.assertEqual(ECCPoint(0xfe, 0x130, ecc), R)

        # ecc = ECC(19, 77, 307, 331)
        # 上の楕円曲線について，下の方法で，
        # y 座標が同じになりx座標が異なる点が
        # 2 個の場合がないか調べたが，
        # 1 個または 3 個のみであったことに驚愕した。
        # view_all_points is True
        # $ python3 generate_points_on_ec.py | \
        #           awk -F, -e '{print $2}' | \
        #           sort | \
        #           uniq --count | \
        #           sort
        #
        # 目視では辛かったので，以下の改良版でも確認。
        # $ python3 generate_points_on_ec.py | \
        #           awk -F, -e '{print $2}' | \
        #           sort | \
        #           uniq --count | \
        #           awk -e '{print $1}' | \
        #           sort | \
        #           uniq

    def test_ecc_the_group_low(self):
        # http://en.wikipedia.org/wiki/Elliptic_curve
        # The group law
        ecc = ECC(2, -1, 7, 11)
        O = ECCPoint(0, 0, ecc, is_infinity=True)

        # case 1.
        P = ECCPoint(1, 3, ecc)
        Q = ECCPoint(4, 1, ecc)
        R_ = P + Q
        R = ECCPoint(R_.x, -R_.y, ecc)
        self.assertEqual(O, P + Q + R)
        P = Q = R = P_ = R_ = None

        # case 2.
        Q = ECCPoint(1, 3, ecc)
        P_ = Q + Q
        P = ECCPoint(P_.x, -P_.y, ecc)
        self.assertEqual(O, P + Q + Q)
        P = Q = R = P_ = R_ = None

        # case 3.
        P = ECCPoint(1, 3, ecc)
        Q = ECCPoint(P.x, -P.y, ecc)
        self.assertEqual(O, P + Q + O)
        P = Q = R = P_ = R_ = None

        # case 4.
        # D.N.E in ECC.
        # no need to think about Elliptic Curve.
        # no maybe find a order which is even.
        # but it's tired ?
        # Therefore I never try to find it !.
        pass

    # complex test ############################################################
    def test_ECCPoint_div(self):
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

    def test_ECCPoint_mul_by_negative_value(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)

        with self.assertRaises(ValueError) as raiz:
            P8 * -1
        args = raiz.exception.args
        self.assertEqual('number(=-1) must be positive value.', args[0])

    def test_ECCPoint_mul_honest(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        P8_75 = ecc.mul_honest(P8, 75)
        self.assertEqual(P26, P8_75)

    def test_ECCPoint_mul_fast(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        P8_75 = ecc.mul_fast(P8, 75)
        self.assertEqual(P26, P8_75)

    def test_ECCPoint_mul(self):
        ecc = ECC(19, 77, 307, 331)
        P8 = ECCPoint(7, 218, ecc)
        P26 = ECCPoint(20, 274, ecc)

        Q = P8 * 75
        self.assertEqual(P26, Q)

        R = 75 * P8
        self.assertEqual(P26, R)

        S = P8
        S *= 75
        self.assertEqual(P26, S)

    # util test ###############################################################
    def test_collect_all_points(self):
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

    def test_calc_pair_of_xy_Nones(self):
        ecc = ECC(2, -1, 7, 11)

        with self.assertRaises(ECCPointError) as raiz:
            ecc.calc_pair_of_xy(x=None, y=None)

class TestHelper(unittest.TestCase):
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
