# author: 梅濁酒(umedoblock)
# references: 妖精現実フェアリアル http://deztecc.jp/x/05/faireal/23-index.html
#           : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

import math
import random

# umm...
if __name__ == '__main__':
    import lib
else:
    from . import lib

__all__ = [
    'ECC', 'ECCPoint', 'ECCPointError', 'ECCGetItem',
    'generate_random', 'gcdext'
]

def ECCGetItem(bit):
    # test data from http://www.rfc-editor.org/rfc/rfc5903.txt
    # make ecc.
    if bit == 256:
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
        G256 = ECCPoint(gx, gy, ecc256)
        return ecc256, G256
    else:
        raise ValueError('invalid bit(={})'.format(bit))

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

def generate_random(rand_max):
    d = generate_random.__dict__
    if not 'randrange' in d:
        d['randrange'] = random.SystemRandom().randrange
    return d['randrange'](1, rand_max)

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

    def __str__(self):
        if self.isinf():
            return '(inf, inf)'
        return '(0x{:x}, 0x{:x})'.format(self.x, self.y)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __gt__(self, other):
        if self.x > other.x:
            return True
        elif self.x < other.x:
            return False
        # now, self.x == other.x
        if self.y > other.y:
            return True
        else:
            # now,
            #  self.x == other.x and
            # (self.y < other.y or self.y == other.y)
            return False

    def __ge__(self, other):
        return self > other or self == other

    def __lt__(self, other):
        if self.x < other.x:
            return True
        elif self.x > other.x:
            return False

        # now, self.x == other.x
        if self.y < other.y:
            return True
        else:
            # now,
            #  self.x == other.x and
            # (self.y > other.y or self.y == other.y)
            return False

    def __le__(self, other):
        return self < other or self == other

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
        '''y ^ 2 = x ^ 3 + a * x + b.'''
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

        a = '{} 0x{:x} * x'.format(sign_a, abs_a)
        b = '{} 0x{:x}'.format(sign_b, abs_b)

        return 'y ^ 2 = x ^ 3 {} {}'.format(a, b)

class ECC(EC):

    def __init__(self, a, b, prime, order=0):
        '''y ^ 2 = x ^ 3 + a * x + b (mod prime, order).'''
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
        lmd %=  self.prime
        x3 = lmd ** 2 - x1 - x2
        y3 = lmd * (x1 - x3) - y1
        x3 %= self.prime
        y3 %= self.prime

      # print('double_y1_inv(={:d}) = (2*{:d}, {:d})'.
      #         format(double_y1_inv, y1, self.prime))
      # print('lmd(={:d}) = (3 * ({:d} ** 2) + {:d}) * {:d}'.
      #         format(lmd, x1, self.a, double_y1_inv))
      # print('x3(={:d}) = lmd(={:d}) ** 2 - x1(={:d}) - x2(={:d})'.
      #         format(x3, lmd, x1, x2))
      # print('y3(={:d}) = lmd(={:d}) * (x1(={:d}) - x3(={:d})) - y1(={:d})'.
      #         format(y3, lmd, x1, x3, y1))
      # print('x3(={:d}), y3(={:d})'.format(x3, y3))

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

    def div(self, P, num):
        gcd, num_inverse, order_ = gcdext(num, self.order)
        return num_inverse * P

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

        left = pow(point.y, 2, self.prime)
        powm_x_3 = pow(point.x, 3, self.prime)
        right = (powm_x_3 + self.a * point.x + self.b) % self.prime

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

        if not lib.is_prime(order):
            raise RuntimeWarning(('order(={}) '
                                  'is not prime number.').format(order))

        self.order = order
        return order

    def collect_all_points(self):
        points = []
        for x in range(self.prime):
            new_points = self.calc_pair_of_xy(x=x)
          # print("new_points =")
          # print(new_points)
            if new_points:
                points.extend(new_points)
        point_at_infinity = ECCPoint(0, 0, self, is_infinity=True)
        points.append(point_at_infinity)

      # print("points =", points)
        return frozenset(points)

    def calc_pair_of_xy(self, x=None, y=None):
        if (x, y) == (None, None):
            raise ECCPointError('x and y are None.')

      # print(f'x={x}, y={y}')
        points = []
      # y ^ 2 = x ^ 3 + a * x + b (mod prime).
        if x is not None:
          # ignore y.
          # y ^ 2 = x ^ 3 + a * x + b
            y_square = (x ** 3 + self.a * x + self.b) % self.prime
            # x=1, a=2, b=-1, p=7, order=11
            # y_square = 1 ** 3 + 2 * 1 + -1
            #          = 1 + 2 + -1
            #          = 2, 9
            # y        = 3
            # x=3, a=2, b=-1, p=7, order=11
            # y_square = 3 ** 3 + 2 * 3 + -1
            #          = 27 + 6 - 1
            #          = 32, 25
            for i in range(self.prime):
                yy = y_square + i * self.prime
                root_y = int(math.sqrt(yy))
                if yy == root_y ** 2:
                    y1 = root_y % self.prime
                    y2 = self.prime - y1
                    points.append(ECCPoint(x, y1, self))
                    points.append(ECCPoint(x, y2, self))
                    break
        else:
            # y is num
            # x ^ 3 + a * x = y ^ 2 - b
            # x * (x ^ 2 + a) = y ^ 2 - b
          # print(f"y={y}, self.b={self.b}, x={x}")
            y2_b = y ** 2 - self.b
            for x in range(self.prime):
                left = x * (x ** 2 + self.a)
                if left % self.prime == y2_b % self.prime:
                    x %= self.prime
                    y %= self.prime
                    P = ECCPoint(x, y, self)
                    points.append(P)

        return points

    def __str__(self):
        ss = super().__str__()
        return (ss + \
             ' (mod 0x{:x}, order 0x{:x})').format(self.prime, self.order)

    def __eq__(self, other):
        if  self.a == other.a and \
            self.b == other.b and \
            self.prime == other.prime and \
            self.order == other.order:
            return True
        else:
            return False

class ECCPoint(Point):
    def __init__(self, x, y, ecc, is_infinity=False):
        super().__init__(x, y)
        self.ecc = ecc
        self._is_infinity = is_infinity

        if is_infinity:
            self.x = self.y = float('inf')
        else:
            self.x %= self.ecc.prime
            self.y %= self.ecc.prime
        if not ecc.exists_with(self):
            raise ECCPointError('{} is not on {}.'.format(self, ecc))

    def isinf(self):
        return self._is_infinity

    def __rmul__(self, other):
        return self.__mul__(other)

    def __mul__(self, other):
        if other < 0:
            raise ValueError('number(={}) must be positive value.'. \
                              format(other))
        return self.ecc.mul_fast(self, other)

    def __add__(self, other):
        return self.ecc.add(self, other)

    def __truediv__(self, num):
        return self.ecc.div(self, num)

    def __eq__(self, other):
        if self.ecc != other.ecc:
            message = '\n'
            message += '{} and\n'.format(self.ecc)
            message += '{}\n'.format(other.ecc)
            message += 'are different ECC.\n'
            message += 'Therefore, __eq__() cannot compare '
            message += '{} with {}.'.format(self, other)
            raise ECCPointError(message)

        if self.isinf() and other.isinf():
            return True
        elif self.isinf() != other.isinf():
            return False

        return super().__eq__(other)

    def __repr__(self):
        return f"({self.x}, {self.y})"

    def __bool__(self):
        return not self.isinf()

    def __hash__(self):
        if self.isinf():
            return -1
        else:
            value = self.ecc.prime * self.y + self.x
            return value

class ECCPointError(BaseException):
    pass

if __name__ == '__main__':
    # data from http://www.rfc-editor.org/rfc/rfc5903.txt
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
    ecc = ECC(a, b, prime, order)

    # make generator.
    gx = int.from_bytes(bytes.fromhex(
                '6B17D1F2 E12C4247 F8BCE6E5 63A440F2'
                '77037D81 2DEB33A0 F4A13945 D898C296'), 'big')
    gy = int.from_bytes(bytes.fromhex(
                '4FE342E2 FE1A7F9B 8EE7EB4A 7C0F9E16'
                '2BCE3357 6B315ECE CBB64068 37BF51F5'), 'big')
    G = ECCPoint(gx, gy, ecc)

    print('ecc =', ecc)
    print('G =', G)
