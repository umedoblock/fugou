from _camellia import _Camellia
from _camellia import BLOCK_SIZE
import random

def _calc_size(text_size, block_size):
    snip_size = text_size % block_size
    if(snip_size == 0):
        padding_size = block_size
    else:
        padding_size = block_size - snip_size
    cipher_size = block_size + text_size + padding_size
    return cipher_size, snip_size, padding_size

class Camellia:
    def __init__(self, key, key_size):
        if key_size not in (128, 192, 256):
            raise ValueError('key_size must be 128, 192 or 256.')
        if not isinstance(key, bytes):
            raise TypeError('type(key) must be bytes.')
        if len(key) * 8 < key_size:
            raise ValueError('len(key) * 8 must be longer than key_size.')

        self.cm = _Camellia(key, key_size)

    def encrypt(self, text):
        text_size = len(text)
        if text_size < BLOCK_SIZE:
            raise ValueError('text length must be longer than 16 octets.')
        return self.cm._encrypt(text)

    def decrypt(self, cipher):
        cipher_size = len(cipher)
        if cipher_size < BLOCK_SIZE:
            raise ValueError('cipher length must be longer than 16 octets.')
        return self.cm._decrypt(cipher)

    def encrypt_cbc(self, text, iv=None, text_size=None):
        if text_size:
            if text_size > len(text):
                raise ValueError('len(text) must be longer than text_size')
        else:
            text_size = len(text)

        if iv:
            if len(iv) < BLOCK_SIZE:
                raise ValueError('iv length must be longer than 16 octets.')
        else:
            # random_max = 1 << (BLOCK_SIZE * 8)
            # random_number = random.randrange(0, random_max)
            random_number = random.getrandbits(BLOCK_SIZE * 8)
            iv = int.to_bytes(random_number, BLOCK_SIZE, 'big')

        # print('self.cm._encrypt_cbc(iv, text, text_size)')
        return self.cm._encrypt_cbc(iv, text, text_size)

    def decrypt_cbc(self, cipher):
        cipher_size = len(cipher)
        if not cipher_size:
            raise ValueError('len(cipher) must be greater than zero')
        if cipher_size % BLOCK_SIZE:
            raise ValueError('len(cipher) must be multiply by BLOCK_SIZE')

        # print('self.cm._decrypt_cbc(iv, cipher, cipher_size)')
        return self.cm._decrypt_cbc(cipher, cipher_size)

if __name__ == '__main__':
    key_size = 128
    k = b'\00' * (key_size // 8)

    cm = Camellia(k, key_size)
    m = b'\00' * BLOCK_SIZE
    c = cm.encrypt(m)
    d = cm.decrypt(c)

    print('m =', m)
    print('c =', c)
    print('d =', d)
    print('m == d is', m == d)
