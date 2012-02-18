# Copyright 2011 梅どぶろく(umedoblock)
import datetime
import math
import copy

import unittest
import sys, os
dname = os.path.dirname(os.path.abspath(__file__))
# print('dname =', dname)
libdir = os.path.join(dname, '..')
# print('libdir =', libdir)
sys.path.insert(0, libdir)
from par2 import Par2
from par2 import Par2Archive

def run():
    MB = 1
    data_size = MB * 1024 ** 2
#   KB = 128
#   data_size = KB * 1024 ** 1
    data = b'\00' * data_size
    mb = data_size / 1024 ** 2

    for bits in (8, 16):
        print('{} bits symbol {:f} MB memory encode/decode speed'.
                format(bits, mb))
        for redundancy in (15,):
            archive = Par2Archive(bits, redundancy)
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
            archive.slots[:archive.par2.redundancy] = \
                [None] * archive.par2.redundancy

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
    import cProfile
    cProfile.run('run()')
