# author: 梅どぶろく(umedoblock)
# references: 妖精現実フェアリアル http://deztecc.jp/x/05/faireal/23-index.html
#           : Elliptic curve       http://en.wikipedia.org/wiki/Elliptic_curve
# Copyright 平成24年(2012)

import datetime
import math
import copy
import argparse

import unittest
import sys, os
dname = os.path.dirname(os.path.abspath(__file__))
# print('dname =', dname)
libdir = os.path.join(dname, '..')
# print('libdir =', libdir)
sys.path.insert(0, libdir)

from ecc import *
from ecdsa import *
from ecdh import *

parser = argparse.ArgumentParser(description='speed test for ecdh and ecdsa.')

parser.add_argument('--ecdsa-times', metavar='N', dest='ecdsa_times',
                   type=int, nargs='?', default=100,
                   help='tiems (default: 100)')

parser.add_argument('--ecdh-times', metavar='N', dest='ecdh_times',
                   type=int, nargs='?', default=100,
                   help='tiems (default: 100)')

parser.add_argument('--prime_length', metavar='N', dest='prime_length',
                   type=int, nargs='?', default=256,
                   help='prime length (default: 256)')

parser.add_argument('--profile', dest='profile',
                   action='store_true', default=False,
                   help='use profile ? (default: False)')

def do_ecdsa_sign(alice, ecdsa_times, hexdigest):
    # signer: alice
    # alice needs judt to set private key.
    # alice doesn't need to compute public key for signature.
    # however bob needs alice's public key to verify signature.

    # alice doesn't need public key for signature.
    for i in range(ecdsa_times):
        # convert hexdigest to int.
        h = int.from_bytes(bytes.fromhex(hexdigest), 'big')
        r, s = alice.sign(h)

    alice.compute_public_key()
    return r, s, alice.get_public_key()

def do_ecdsa_verify(bob, ecdsa_times, hexdigest, r, s, public_key):
    # verifier: bob
    # bob must get alice's public key.
    # Therefore alice compute public key and send it to bob.
    for i in range(ecdsa_times):
        # convert hexdigest to int.
        h = int.from_bytes(bytes.fromhex(hexdigest), 'big')
        result = bob.verify(r, s, h, public_key)
        if not result:
            raise ECDSAError('bob got INVALID signature.',
                              r, s, h, alice_public_key)

def do_ecdsa(prime_length, ecdsa_times):
    ecc, generator = ECCGetItem(prime_length)

    alice = ECDSA(generator)
    # alice needs judt to set private key.
    # w = int.from_bytes(bytes.fromhex(
    #             'DC51D386 6A15BACD E33D96F9 92FCA99D'
    #             'A7E6EF09 34E70975 59C27F16 14C88A7F'), 'big')
    # alice.set_private_key(private_key=w)
    # you can use set_private_key() like a below.
    # private_key is initialized by random number.
    alice.set_private_key()

    # calculate digest.
    import hashlib
    m = hashlib.sha1()
    m.update(b'abc')
    hexdigest = m.hexdigest()

    # sign turn
    st = datetime.datetime.now()
    r, s, alice_public_key = do_ecdsa_sign(alice, ecdsa_times, hexdigest)
    en = datetime.datetime.now()
    do_seconds =  (en - st).total_seconds()
    performance('ecdsa sign()', do_seconds, prime_length, ecdsa_times)

    # verify turn
    bob = ECDSA(generator)
    st = datetime.datetime.now()
    do_ecdsa_verify(bob, ecdsa_times, hexdigest, r, s, alice_public_key)
    en = datetime.datetime.now()
    do_seconds =  (en - st).total_seconds()
    performance('ecdsa verify()', do_seconds, prime_length, ecdsa_times)

def performance(message, do_seconds, prime_length, times):
    do_speed = times / do_seconds
    print('{:20s} {} signatures.'.format(message, times))
    print('      {:.2f} seconds.'.format(do_seconds))
    print('speed {:.2f} signs / second.'.format(do_speed))
    print('      {:.2f} seconds / signature.'.format(1 / do_speed))

def run(prime_length, ecdh_times, ecdsa_times):
    do_ecdsa(prime_length, ecdsa_times)

if __name__ == '__main__':
    args = parser.parse_args()
    prime_length = args.prime_length
    ecdh_times = args.ecdh_times
    ecdsa_times = args.ecdsa_times
    profile = args.profile

    if profile:
        import cProfile
        cProfile.run('run(prime_length, ecdh_times, ecdsa_times)')
    else:
        run(prime_length, ecdh_times, ecdsa_times)
