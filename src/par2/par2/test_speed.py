# Copyright 2011 梅どぶろく(umedoblock)
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
from par2 import Par2
from par2 import Par2Archive

parser = argparse.ArgumentParser(description='speed test for par2.')

parser.add_argument('--megabytes', metavar='N', dest='mb',
                   type=int, nargs='?', default=0,
                   help='mega bytes (default: 0)')

parser.add_argument('--kilobytes', metavar='N', dest='kb',
                   type=int, nargs='?', default=256,
                   help='kilo bytes (default: 256)')

parser.add_argument('--division', metavar='N', dest='division',
                   type=int, nargs='?', default=15,
                   help='kilo bytes (default: 15)')

def run():
    args = parser.parse_args()
    MB = args.mb
    KB = args.kb
    division = args.division

    mega = MB * 1024 ** 2
    kilo = KB * 1024 ** 1
    data_size = mega + kilo
    data = b'\00' * data_size

    mb = data_size / 1024 ** 2

    print('mega = {}, kilo = {}, data_size = {}, mb = {}'. \
            format(mega, kilo, data_size, mb))

    bitss = (8, 16)
    bitss = (24,)
    bitss = (8, 16, 24)
    for bits in bitss:
        print('{} bits symbol {} division {:f} MB memory encode/decode speed'.
                format(bits, division, mb))
        archive = Par2Archive(bits, division)
        archive.take_data(data)
        # make parities
        s = datetime.datetime.now()
        archive.make_parities()
        e = datetime.datetime.now()
        encode_seconds =  (e - s).total_seconds()
        encode_speed = data_size / 1024 ** 2 / encode_seconds
        print('encode {:.2f} seconds, speed {:.2f} MB/sec'.
            format(encode_seconds, encode_speed))

        # delete all parts
        archive.slots[:archive.par2.division] = \
            [None] * archive.par2.division

        # fix up
        s = datetime.datetime.now()
        raw_data = archive.fix_up_data()
        e = datetime.datetime.now()
        decode_seconds =  (e - s).total_seconds()
        decode_speed = data_size / 1024 ** 2 / decode_seconds
        print('decode {:.2f} seconds, speed {:.2f} MB/sec'.
            format(decode_seconds, decode_speed))

        print('data == raw_data is', data == raw_data)
        print()

if __name__ == '__main__':
    run()
  # import cProfile
  # cProfile.run('run()')
