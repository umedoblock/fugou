try:
    from _gcdext import gcdext

except ImportError as e:
    print('cannot import _gcdext')
    print('reason: ', e.args[0])
    class eccError(BaseException):
        pass

__all__ = [
    'EC', 'ECPoint', 'gcdext',
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

class EC(object):
    def __init__(self, g, p, a, b):
        self.g = g
        self.p = p
        self.a = a
        self.b = b
class ECPoint(object):
    def __init__(self, ec, x, y):
        self.ec = ec
        self.x = x
        self.y = y

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
        print('__add__({}, {})'.format(id(self), id(other)))
        x = self.x + other.x
        y = self.y + other.y
        obj = Point(x, y)
        return obj

    def __iadd__(self, other):
        print('__iadd__({}, {})'.format(id(self), id(other)))
        self.x += other.x
        self.y += other.y
        return self

    def __str__(self):
        return '(x, y) = ({}, {})'.format(self.x, self.y)

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
