# author: 梅どぶろく(umedoblock)
# reference: 妖精現実フェアリアル http://deztec.jp/x/05/faireal/23-index.html
#          : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

try:
    from _gcdext import gcdext

except ImportError as e:
    print('cannot import _gcdext')
    print('reason: ', e.args[0])

__all__ = [
    'Point', 'EC', 'ECPoint', 'gcdext',
    'ECPointError'
]

from sys import modules
def have_gcdext_c_extension():
    return '_gcdext' in modules

if not have_gcdext_c_extension():
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

class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def is_on(self, ec):
        return ec.exists_with(self)

    def constructs(self, ec):
        return ec.exists_with(self)

    def __str__(self):
        return '({}, {})'.format(self.x, self.y)

class EC(object):

    def __init__(self, a, b, prime, order):
        self.a = a
        self.b = b
        self.prime = prime
        self.order = order

    def exists_with(self, point):
        left = point.y ** 2
        right = point.x ** 3 + self.a * point.x + self.b
        left %= self.prime
        right %= self.prime
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

        return 'y ^ 2 = x ^ 3 {} {} (mod {})'.format(a, b, self.prime)

    def __eq__(self, other):
        if  self.a == other.a and \
            self.b == other.b and \
            self.prime == other.prime and \
            self.order == other.order:
            return True
        else:
            return False

class ECPoint(Point):
    def __init__(point, x, y, ec, is_infinity=False):
        super().__init__(x, y)
        if is_infinity or ec.exists_with(point):
            point.ec = ec
            point.is_infinity = is_infinity
        else:
            point.is_infinity = False
            raise ECPointError('{} is not on {}.'.format(point, ec))

    def __rmul__(self, other):
        print('__rmul2__({}, {})'.format(id(self), id(other)))
        return self.__mul__(other)

    def __imul__(self, other):
        print('__imul__({}, {})'.format(id(self), id(other)))
        self.x *= other
        self.y *= other
        return self

    def __mul__(self, other):
        print('__mul__({}, {})'.format(id(self), id(other)))
        x = self.x * other
        y = self.y * other
        obj = Point(x, y)
        return obj

    def __add__(self, other):
        self._check_other_on_ec(other)

        if self.is_infinity or other.is_infinity:
            if self.is_infinity:
                point = other
            else:
                point = self
            return ECPoint(point.x, point.y, point.ec, point.is_infinity)

        x1, y1 = self.x, self.y
        x2, y2 = other.x, other.y
        if self == other:
            gcd, double_y1_inv, _n = gcdext(2 * y1, self.ec.prime)
            lmd = (3 * (x1 ** 2) + self.ec.a) * double_y1_inv
        elif x1 == x2:
            # not equal to y1 and y2
            point_at_infinity = ECPoint(0, 0, self.ec, is_infinity=True)
            return point_at_infinity
        else:
            gcd, x2_x1_inv, _n = gcdext(x2 - x1, self.ec.prime)
            lmd = (y2 - y1) * x2_x1_inv
        x3 = lmd ** 2 - x1 - x2
        y3 = lmd * (x1 - x3) - y1
        x3 %= self.ec.prime
        y3 %= self.ec.prime

        ecp = ECPoint(x3, y3, self.ec)

        return ecp

    def __eq__(self, other):
        self._check_other_on_ec(other)

        x_eq = (self.x % self.ec.prime) == (other.x % other.ec.prime)
        y_eq = (self.y % self.ec.prime) == (other.y % other.ec.prime)

        return x_eq and y_eq

    def __iadd__(self, other):
        print('__iadd__({}, {})'.format(id(self), id(other)))
        self.x += other.x
        self.y += other.y
        return self

    def __str__(self):
        if self.is_infinity:
            return '0'
        else:
            return super().__str__()

    def _check_other_on_ec(self, other):
        if self.ec != other.ec:
            raise ECPointError('\n{} is not\n{}.'.format(self.ec, other.ec))

class ECPointError(BaseException):
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
