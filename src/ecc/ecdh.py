# author: 梅濁酒(umedoblock)
# references: 妖精現実フェアリアル http://deztecc.jp/x/05/faireal/23-index.html
#           : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

from ecc import *

__all__ = ['ECDH', 'ECDHError']

class ECDH(object):
    '''ECDH'''

    def __init__(self, generator):
        if not isinstance(generator, ECCPoint):
            raise TypeError('generator must be ECCPoint type.')
        self._generator = generator
        self._private_key = 0
        self._public_key = ECCPoint(0, 0, generator.ecc, is_infinity=True)
        self._secret_key = ECCPoint(0, 0, generator.ecc, is_infinity=True)
        self._computed_public_key = False
        self._client_public_key = \
            ECCPoint(0, 0, generator.ecc, is_infinity=True)

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

    def make_secret_key(self, client_public_key):
        self._generator.ecc.exists_with(client_public_key)

        if not self._private_key or not self._public_key:
            raise ValueError('You must have _private_key and _public_key.')
        if not self._computed_public_key:
            if self._private_key * self._generator == self._public_key:
                self._computed_public_key = True
            else:
                raise ECDHError('mismatch _private_key and _public_key.')

        self._secret_key = self._private_key * client_public_key

    def get_public_key(self):
        if not self._public_key:
            raise ValueError('_public_key is empty value.')
        return self._public_key

    def get_private_key(self):
        if not self._private_key:
            raise ValueError('_private_key is empty value.')
        return self._private_key

    def get_secret_key(self):
        if not self._secret_key:
            raise ValueError('_secret_key is empty value.')
        return self._secret_key

    def __del__(self):
        # TODO: change _private_key and _secret_key value in C language.
        del(self._private_key, self._secret_key)

class ECDHError(BaseException):
    pass

if __name__ == '__main__':
    ecc = ECC(19, 77, 307, 331)
    generator = ECCPoint(7, 218, ecc)
    alice = ECDH(generator)
    bob = ECDH(generator)

    alice.set_private_key()
    bob.set_private_key()

    alice.compute_public_key()
    alice_public_key = alice.get_public_key()

    bob.compute_public_key()
    bob_public_key = bob.get_public_key()

    alice_secret_key = alice.make_secret_key(bob_public_key)
    bob_secret_key = bob.make_secret_key(alice.get_public_key())

    if alice_secret_key == bob_secret_key:
        print('Alice and Bob succeed to share secret key.')
    else:
        print('Alice and Bob failed to share secret key.')
