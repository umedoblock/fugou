from _camellia import _Camellia
from _camellia import BLOCK_SIZE

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
