import math

class MontgomeryError(BaseException):
    def __init__(self, modulus):
        self.modulus = modulus
        message = '{} is not appreciate modulus.'.format(self.modulus)
        message += ' Modulus must be positive integer and odd number.'
        self.message = message

    def __str__(self):
        return self.message

class Montgomery:
    def __init__(self, n):
        if n <= 0 or not isinstance(n, int) or n % 2 == 0:
            raise MontgomeryError(n)

        self.n = n
        self.nbit = (int(math.log(n, 2)) + 1) + 1
        # print('nbit =', self.nbit)
        self.r = 1 << self.nbit
        self.r_square = self.r ** 2 % self.n

        x, y, gcd = gcdext(self.r, self.n)
        # print(self.r, self.n, x, y, gcd)
        self.r_ = x
        # x, y, gcd = gcdext(self.n, self.r)
        x = self.r - y

        # nx - ry = 1
        # anx - ary = a
        self.ffffffff = self.r - 1
        # self.n_ = x * (self.r - 1) % self.r
        self.n_ = x * self.ffffffff & self.ffffffff
        # print('(a, b, x, y, gcd =', self.n, self.r, x, y, gcd)

        self.c_extension = False

    def mul(self, a, b):
        # return a * b * r_ mod n
        t = a * b
        # m = (t % self.r) * self.n_ % self.r
        m = (t & self.ffffffff) * self.n_ & self.ffffffff
        # k = (t + m * self.n) // self.r
        abr_ = (t + m * self.n) >> self.nbit

        if abr_ > self.n:
            abr_ -= self.n
        return abr_

    def pow(self, a, x):
        ''' y = a ^ x mod n '''
        if not x:
            return 1
        A = self.Fr(a)
        flg = 1 << (x.bit_length() - 1)
        Y = A
        flg >>= 1
        while flg:
            Y = self.mul(Y, Y)
            if x & flg:
                Y = self.mul(Y, A)
            flg >>= 1
        y = self.Fr_inv(Y)
        return y

    def pow_slow(self, a, x):
        A = self.Fr(a)
        ANS = A
        for i in range(x - 1):
            ANS = self.mul(A, ANS)
        ans = self.Fr_inv(ANS)
        return ans

    def Fr(self, a):
      # A_ = a * self.r % self.n
        A = self.mul(a, self.r_square)
      # if A != A_:
      #     raise()
        return A

    def Fr_inv(self, A):
      # a_ = A * self.r_ % self.n
        a = self.mul(A, 1)
      # if a != a_:
      #     raise()
        return a

def gcdext(a, b):
    '''return (x, y, gcd). ax - by = gcd'''

    x, y, gcd = 1, 0, a
    x1, y1, z1 = 0, 1, b

    while z1 > 0:
        div, z2 = divmod(gcd, z1)
        div_x1 = div * x1
        x2 = x - div_x1

        div_y1 = div * y1
        y2 = y - div_y1

        x = x1
        y = y1
        gcd = z1
        x1 = x2
        y1 = y2
        z1 = z2
    # print('x, y, gcd, a, b = {}, {}, {}, {}, {}'.format(x, y, gcd, a, b))

    if x < 0:
        x = b + x
        y = a - y
    else:
        y = abs(y)
    return x, y, gcd
