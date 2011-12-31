import unittest

from __init__ import Montgomery
from __init__ import MontgomeryError
from __init__ import gcdext

class TestMontgomery(unittest.TestCase):
    def setUp(self):
        pass

    def test__init__(self):
        Montgomery(101)

        invalid_moduluses = [0, 100, -101, 101.0]
        for invalid_modulus in invalid_moduluses:
            msg = '{} is not appreciate modulus. '.format(invalid_modulus)
            msg += 'Modulus must be positive integer and odd number.'
            with self.assertRaisesRegex(MontgomeryError, msg):
                Montgomery(invalid_modulus)

    def test_gcd_ext(self):
        x, y, gcd = gcdext(5, 7)
        self.assertTupleEqual((3, 2, 1), (x, y, gcd))

        x, y, gcd = gcdext(7, 5)
        self.assertTupleEqual((3, 4, 1), (x, y, gcd))

        x, y, gcd = gcdext(101, 1007)
        self.assertTupleEqual((339, 34, 1), (x, y, gcd))

        x, y, gcd = gcdext(1007, 101)
        self.assertTupleEqual((67, 668, 1), (x, y, gcd))

    def test_mul_basic(self):
        mng = Montgomery(101)

        a, b = 53, 47
        abr_ = mng.mul(a, b)
        self.assertEqual(a * b * mng.r_ % mng.n, abr_)
        c = mng.Fr(abr_)
        self.assertEqual(a * b % mng.n, c)

    def test_mul_1(self):
        mng = Montgomery(101)

        a, b = 53, 47
        ar = mng.Fr(a)
        br = mng.Fr(b)
        abr = mng.mul(ar, br)
        c = mng.mul(abr, 1)
        self.assertEqual(67, c)

    def test_mul_2(self):
        mng = Montgomery(101)

        a, b = 53, 47
        ar = mng.mul(a, mng.r_square)
        c = mng.mul(ar, b)
        self.assertEqual(67, c)

    def test_Fr(self):
        mng = Montgomery(101)

        a = 53
        ar = mng.Fr(a)
        self.assertEqual(34, ar)
        a_ = mng.Fr_inv(ar)
        self.assertEqual(a, a_)

    def test_pow_nerbous(self):
        mng = Montgomery(101)
        ans = mng.pow(2, 1)
        self.assertEqual(2, ans)
        ans = mng.pow(2, 0)
        self.assertEqual(1, ans)

    def test_pow_normal(self):
        mng = Montgomery(101)
        # print(pow(2, 44, 101))

        ans = mng.pow(2, 44)
        self.assertEqual(71, ans)

    def test_pow_big_number(self):
        n = ('fa5688706cd7b1ae454c3065f7fe796bd27340a57cbc6708d07a5cd85149301'
             'a187bb7e13ae5f6aef664e8bfbb2eba351029d44ce0c68b3edb88c18d22b167'
             'e01099d6a94dbe75f21e1692413ac0602677e256696cd53c0ff30de29356277'
             '155c4541b31cc45f05b8ed05ee294e5fb556ac0c64f33b16939f3f55c92b86a'
             '3583')
        n = int(n, 16)

        x = ('ccd8009d78177f9236b3b5c7fbf25bc86bfec81d6d699e072387187cbefd659'
             'e8de6ae2786c783b52cc507b9540fefd7093d021ec7ee433062e002cc82cb14'
             '9a5d2f796771b65165f69df96dc441095c28b7efd59190fccdd79094fe19b05'
             '115a0a0bbd0ac7957c6f2c9734190fa45320d048de9d5ccc01994a71edf6f3e'
             'd471')
        x = int(x, 16)

        a = 2
        expected_y = pow(a, x, n)

        mng = Montgomery(n)
        y = mng.pow(a, x)

        self.assertEqual(expected_y, y)

if __name__ == '__main__':
    unittest.main()
