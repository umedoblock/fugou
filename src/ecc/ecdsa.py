# author: 梅濁酒(umedoblock)
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
            return ECDSA.INVALID

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

class ECDSAError(BaseException):
    def __init__(self, message, r, s, h, public_key):
        self.message = message
        self.r = r
        self.s = s
        self.h = h
        self.public_key = public_key

    def __str__(self):
        ss = (
        '{}\n'
        'r = 0x{:x}\n'
        's = 0x{:x}\n'
        'h = 0x{:x}\n'
        'public_key = {}\n') \
        .format(self.message, self.r, self.s, self.h, self.public_key)
        return ss

if __name__ == '__main__':
    ecc256, generator256 = ECCGetItem(256)

    # signer: alice
    # verifier: bob
    # alice needs just to set private key.
    # alice doesn't need to compute public key for signature.
    # however bob needs alice's public key to verify signature.
    alice = ECDSA(generator256)
    bob = ECDSA(generator256)

    # alice needs just to set private key.
    w = int.from_bytes(bytes.fromhex(
                'DC51D386 6A15BACD E33D96F9 92FCA99D'
                'A7E6EF09 34E70975 59C27F16 14C88A7F'), 'big')
    alice.set_private_key(private_key=w)
    # you can use set_private_key() like a below.
    # private_key is initialized by random number.
    # alice.set_private_key()

    # calculate digest.
    import hashlib
    m = hashlib.sha1()
    m.update(b'abc')
    hexdigest = m.hexdigest()

    # convert hexdigest to int.
    h = int.from_bytes(bytes.fromhex(hexdigest), 'big')

    # alice doesn't need public key for signature.
    r, s = alice.sign(h)

    # bob must get alice's public key.
    # Therefore alice compute public key and send it to bob.
    alice.compute_public_key()
    alice_public_key = alice.get_public_key()
    result = bob.verify(r, s, h, alice_public_key)
    if result:
        print('bob got VALID signature.')
    else:
        raise ECDSAError('bob got INVALID signature.', \
                         r, s, h, alice_public_key)
