from _camellia import _Camellia

class CamelliaError: pass

class Camellia:
    def __init__(self, key, key_size):
        if key_size not in (128, 192, 256):
            raise CamelliaError('key_size must be 128, 192 or 256')
        if not isinstance(key, bytes):
            raise TypeError('type(key) must be bytes')

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
