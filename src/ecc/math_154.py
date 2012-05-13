from ecc import *
if __name__ == '__main__':
    # 点8 の 2倍
    ecc = ECC(19, 77, 307, 331)
    P8 = ECCPoint(7, 218, ecc)
    P = P8 + P8
    print('P8 =', P8)
    print('P =', P)
