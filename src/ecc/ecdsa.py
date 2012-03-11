# author: 梅どぶろく(umedoblock)
# references: ECDSA http://www.rfc-editor.org/rfc/rfc4754.txt
# Copyright 平成24年(2012)

from ecc import *

__all__ = ['ECDSA']

class ECDSA(object):
    '''ECDSA'''

    VALID = True
    INVALID = not VALID

    def __init__(self, generator):
        if not isinstance(generator, ECCPoint):
            raise TypeError('generator must be ECCPoint type.')
        self._generator = generator
        self._private_key = 0
        self._public_key = ECCPoint(0, 0, generator.ecc, is_infinity=True)
        self._computed_public_key = False

    def sign(self, h):
        if not self._private_key:
            raise ValueError('_private_key is empty value.')
        ecc = self._generator.ecc
        w = self.get_private_key()
        while True:
            k = generate_random(ecc.order)
            # print('k = {:x}'.format(k))
            gk = k * self._generator
            r = gk.x
            if r:
                gcd, k_inv, n_inv = gcdext(k, ecc.order)
                rw = r * w % ecc.order
                s = (h + rw) * k_inv % ecc.order
                if s:
                    break

        k = 0
        return r, s

    def verify(self, r, s, h, public_key):
        ecc = self._generator.ecc
        if 1 <= r <= ecc.order - 1 and \
           1 <= s <= ecc.order - 1:
           pass
        else:
            return INVALID

        gw = public_key
        gcd, s_inv, n_inv = gcdext(s, ecc.order)
        u = h * s_inv % ecc.order
        v = r * s_inv % ecc.order
        gu = u * self._generator
        gwv = v * gw
        sum_ = gu + gwv

        if sum_.x == r:
            return ECDSA.VALID
        else:
            return ECDSA.INVALID

    def set_private_key(self, private_key=0):
        if not isinstance(private_key, int):
            raise TypeError('private_key must be int type.')
        if private_key:
            self._private_key = private_key
        else:
            self._private_key = generate_random(self._generator.ecc.order)

    def compute_public_key(self):
        self._public_key = self._private_key * self._generator
        self._computed_public_key = True

    def read_public_key(self, public_key):
        self._generator.ecc.exists_with(public_key)
        self._public_key = public_key

    def get_public_key(self):
        if not self._public_key:
            raise ValueError('_public_key is empty value.')
        return self._public_key

    def get_private_key(self):
        if not self._private_key:
            raise ValueError('_private_key is empty value.')
        return self._private_key

    def __del__(self):
        # TODO: change _private_key value in C language.
        self._private_key = None

if __name__ == '__main__':
    pass
