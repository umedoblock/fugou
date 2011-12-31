import unittest
from oraga import Oraga

class TestOraga(unittest.TestCase):
    def test_simple(self):
        a, b, n = 531, 417, 257
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        self.assertEqual((div, mod), ans)

    def test_simple2(self):
        a, b, n = 5031, 9417, 13
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        self.assertEqual((div, mod), ans)

    def test_simple3(self):
        a, b, n = 531, 417, 101
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        self.assertEqual((div, mod), ans)

    def test_simple4(self):
        a, b, n = 53, 47, 101
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        print('self.r =', oraga.r)
        self.assertEqual((div, mod), ans)

    def test_simple5(self):
        a, b, n = 5031, 9417, 3
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        self.assertEqual((div, mod), ans)

    def test_simple6(self):
        a, b, n = 5031, 9417, 2
        div, mod = divmod(a * b, n)
        print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
        oraga = Oraga(n)
        ans = oraga.mul(a, b)
        print('ans =', ans)
        self.assertEqual((div, mod), ans)

if __name__ == '__main__':
    unittest.main()
