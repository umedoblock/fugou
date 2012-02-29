# author: 梅どぶろく(umedoblock)
# references: 妖精現実フェアリアル http://deztecc.jp/x/05/faireal/23-index.html
#           : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

import math

from ecc.collect_primes import is_prime

__all__ = [
    'Point', 'ECC', 'ECCPoint', 'gcdext',
    'ECCPointError'
]

from sys import modules

def gcdext(a, b):
    ''' return (gcd, a, b)
        gcd = a * x - b * y '''

    x, y, gcd  = 1, 0, a
    x1, y1, z1 = 0, 1, b

    while z1 > 0:
        div, z2 = divmod(gcd, z1)

        div_ = div * x1
        x2 = x - div_

        div_ = div * y1
        y2 = y - div_

        x, y, gcd = x1, y1, z1
        x1, y1, z1 = x2, y2, z2

    # print(gcd, x, y)
    if y < 0:
        y = -y
    if x < 0:
        x += b
        y = a - y

    return gcd, x, y

try:
    from _gcdext import gcdext
    print('can import _gcdext')

except ImportError as e:
    print('cannot import _gcdext')
    print('reason: ', e.args[0])

class Line(object):
    pass
'''
    def __init__(self, num):
        self.num = num

    def __str__(self):
        return '{}'.format(self.num)
'''

class Point(Line):
    def __init__(self, x, y):
        '''
        self.x = Line(x)
        self.y = Line(y)
        or
        '''
        self.x = x
        self.y = y

    def is_on(self, ecc, raise_error=True):
        return ecc.exists_with(self, raise_error)

    def constructs(self, ecc, raise_error=True):
        return ecc.exists_with(self, raise_error)

    def __repr__(self):
        return '({}, {})'.format(self.x, self.y)

    def __str__(self):
        return '({}, {})'.format(self.x, self.y)

'''
class Space(Point):
    def __init__(self, x, y, z):
        self.p = Point(x, y)
        self.z = Line(z)
        or
        self.x = Line(x)
        self.y = Line(y)
        self.z = Line(z)
        or
        self.x = x
        self.y = y
        self.z = z
        or
        self.x, self.y = Point(x, y)
        self.z = Line(z)

    def __str__(self):
        return '({}, {}, {})'.format(self.x, self.y, self.z)

'''

class EC(object):
    def __init__(self, a, b):
        ''' y ^ 2 = x ^ 3 + a * x + b.'''
        self.a = a
        self.b = b

    def exists_with(self, point):
        left = point.y ** 2
        right = point.x ** 3 + self.a * point.x + self.b
        return left == right

    def __str__(self):
        if self.a > 0:
            sign_a = '+'
        else:
            sign_a = '-'
        abs_a = abs(self.a)
        if self.b > 0:
            sign_b = '+'
        else:
            sign_b = '-'
        abs_b = abs(self.b)

        a = '{} {} * x'.format(sign_a, abs_a)
        b = '{} {}'.format(sign_b, abs_b)

        return 'y ^ 2 = x ^ 3 {} {}'.format(a, b)

class ECC(EC):

    def __init__(self, a, b, prime, order=0):
        ''' y ^ 2 = x ^ 3 + a * x + b (mod prime).'''
        super().__init__(a, b)
        self.prime = prime
        self.order = order

    def add(self, P, Q):
        self.exists_with(P)
        self.exists_with(Q)

        if P.isinf() or Q.isinf():
            if P.isinf():
                point = Q
            else:
                point = P
            return ECCPoint(point.x, point.y, self, point.isinf())

        x1, y1 = P.x, P.y
        x2, y2 = Q.x, Q.y
        if P == Q:
            gcd, double_y1_inv, _n = gcdext(2 * y1, self.prime)
            lmd = (3 * (x1 ** 2) + self.a) * double_y1_inv
        elif x1 == x2:
            # P is not qual to Q.
            # P and Q are on same y coordinate.
            point_at_infinity = ECCPoint(0, 0, self, is_infinity=True)
            return point_at_infinity
        else:
            gcd, x2_x1_inv, _n = gcdext(x2 - x1, self.prime)
            lmd = (y2 - y1) * x2_x1_inv
        x3 = lmd ** 2 - x1 - x2
        y3 = lmd * (x1 - x3) - y1
        x3 %= self.prime
        y3 %= self.prime

        R = ECCPoint(x3, y3, self)

        return R

    def mul_fast(self, eccp, num):
        flg = 1
        flged_eccp = eccp

        muled = 0
        muled_eccp = ECCPoint(0, 0, self, is_infinity=True)

        if flg & num:
            muled += flg
            muled_eccp += flged_eccp

        flg <<= 1
        flged_eccp += flged_eccp
        while flg <= num:
            if flg & num:
                muled += flg
                muled_eccp += flged_eccp

            flg <<= 1
            flged_eccp += flged_eccp
        return muled_eccp

    def mul_honest(self, eccp, num):
        muled_eccp = eccp
        for i in range(num - 1):
            muled_eccp = muled_eccp + eccp
        return muled_eccp

    def exists_with(self, point, raise_error=True):
        if not isinstance(point, Point):
            raise TypeError('point must be Point or Point subtype.')
        if isinstance(point, ECCPoint):
            if self != point.ecc:
                if raise_error:
                    message = \
                        '{} is not on me(={}), on {}.'.format(
                         point, self, point.ecc)
                    raise ECCPointError(message)
                else:
                    return False
            if point.isinf():
                return True
        left = point.y ** 2
        powm_x_3 = pow(point.x, 3, self.prime)
        right = powm_x_3 + self.a * point.x + self.b
        left %= self.prime
        right %= self.prime
        if left == right:
            return True
        elif raise_error:
            raise ECCPointError('{} is not on {}.'.format(point, self))
        else:
            return False

    def calc_order(self):
        order = 0
        for y in range(self.prime):
            for x in range(self.prime):
                point = Point(x, y)
                if self.exists_with(point, raise_error=False):
                    order += 1
        # for point at infinity
        order += 1
        if not is_prime(order):
            raise RuntimeWarning(('order(={}) '
                                  'is not prime number.').format(order))
        self.order = order

        return order

    def collect_all_points(self):
        points = []
        for y in range(self.prime):
            for x in range(self.prime):
                point = Point(x, y)
                if self.exists_with(point, raise_error=False):
                    eccp = ECCPoint(point.x, point.y, self)
                    points.append(eccp)
        point_at_infinity = ECCPoint(0, 0, self, is_infinity=True)
        points.append(point_at_infinity)

        return frozenset(points)

    def calc_pair_of_xy(self, x=None, y=None):
        on_curve = False
      # y ^ 2 = x ^ 3 + a * x + b (mod prime).
        if x is not None:
            # y ^ 2 = x ^ 3 + a * x + b
            y_square = (x ** 3 + self.a * x + self.b) % self.prime
            y_ = int(math.sqrt(y_square))
            if y_square == (y_ ** 2) % self.prime:
                y = y_
                tup = (x, y)
            else:
                tup = None
        elif y is not None:
            # x ^ 3 + a * x = y ^ 2 - b
            # x * (x ^ 2 + a) = y ^ 2 - b
            y2_b = y ** 2 - self.b
            equal = False
            for x in range(self.prime):
                left = x * (x ** 2 + self.a)
                if left % self.prime == y2_b % self.prime:
                    equal = True
                    break;
            if not equal:
                tup = None
            else:
                tup = (x, y)
        else:
            raise ECCPointError('x and y are None.')
        return tup

    def __str__(self):
        ss = super().__str__()

        return ss + ' (mod {}, order {})'.format(self.prime, self.order)

    def __eq__(self, other):
        if  self.a == other.a and \
            self.b == other.b and \
            self.prime == other.prime and \
            self.order == other.order:
            return True
        else:
            return False

class ECCPoint(Point):
    def __init__(point, x, y, ecc, is_infinity=False):
        super().__init__(x, y)
        point.ecc = ecc
        point._is_infinity = False

        if is_infinity or ecc.exists_with(point):
            point._is_infinity = is_infinity
        else:
            raise ECCPointError('{} is not on {}.'.format(point, ecc))
        if is_infinity:
            point.x = point.y = float('inf')

    def __rmul__(self, other):
        return self.__mul__(other)

    def __mul__(self, other):
        return self.ecc.mul_fast(self, other)

    def __add__(self, other):
        return self.ecc.add(self, other)

    def __eq__(self, other):
        self._check_other_on_ec(other)

        if self.isinf() and other.isinf():
            return True
        elif self.isinf() != other.isinf():
            return False

        x_eq = (self.x % self.ecc.prime) == (other.x % other.ecc.prime)
        y_eq = (self.y % self.ecc.prime) == (other.y % other.ecc.prime)

        return x_eq and y_eq

    def __hash__(self):
        if self.isinf():
            return -1
        else:
            value = self.ecc.prime * self.y + self.x
            return value

    def isinf(self):
        return self._is_infinity

    def _check_other_on_ec(self, other):
        if self.ecc != other.ecc:
            raise ECCPointError('\n{} is not\n{}.'.format(self.ecc, other.ecc))

class ECCPointError(BaseException):
    pass

if __name__ == '__main__':
    p0 = Point(1, 2)
    p1 = Point(3, 4)
    print('p0 =', id(p0))
    print('p1 =', id(p1))
    print('------------------------------------------------')

    p0 += p1
    print('p0 =', id(p0))
    print(str(p0))
    print('------------------------------------------------')

    p2 = p0 + p1
    print('p2 =', id(p2))
    print(str(p0))
    print(str(p2))

    p2 *= 3
    print('p2 =', id(p2))
    print(str(p2))

    p3 = p2 * 3
    print('p3 =', id(p3))
    print(str(p3))

    p4 = 3 * p3
    print('p4 =', id(p4))
    print(str(p4))
    print('------------------------------------------------')
    print(str(p0))
    print(str(p1))
    print(str(p2))
    print(str(p3))
    print(str(p4))
