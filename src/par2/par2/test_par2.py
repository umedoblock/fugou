# Copyright 2011 梅どぶろく(umedoblock)

import unittest
import sys, os
dname = os.path.dirname(os.path.abspath(__file__))
# print('dname =', dname)
libdir = os.path.join(dname, '..')
# print('libdir =', libdir)
sys.path.insert(0, libdir)
from par2 import Par2
from par2 import Par2Archive
from par2 import TAIL_SIZE
from par2 import Par2Error

class TestPar2(unittest.TestCase):
    def setUp(self):
        self.p4 = Par2(4, redundancy=15)

    def test_bits(self):
        Par2(4)
        Par2(8)
      # Par2(16) too slowly
        with self.assertRaises(KeyError):
            Par2(5)

    def test_mul_and_div(self):
        p4 = self.p4
        for a in range(self.p4.w):
            for n in range(p4.gf_max):
                b = n + 1
                mul = p4._mul(a, b)
                c = p4._div(mul, b)
                self.assertEqual(a, c)
              # print(a, b, mul, c)

    def test__mul_matrix_by_e_matrix(self):
        redundancies = {4:15, 8:50, 16:50}
        for bits in (4, 8, 16):
            redundancy = redundancies[bits]
            par2 = Par2(bits, redundancy)
            matrix = []
            for i in range(par2.redundancy):
                foo = [x % par2.w for x in range(i, par2.redundancy + i)]
                matrix.append(foo)
            e_matrix = par2._make_e_matrix()

            muled_matrix = par2._mul_matrixes(matrix, e_matrix)
            self.assertEqual(matrix, muled_matrix)

            muled_matrix = par2._mul_matrixes(e_matrix, matrix)
            self.assertEqual(matrix, muled_matrix)

    def test__solve_inverse_matrix(self):
        redundancies = {4:15, 8:50, 16:50}
        for bits in (4, 8, 16):
            redundancy = redundancies[bits]
            par2 = Par2(bits, redundancy)

            matrix = par2.vander_matrix
            inverse_matrix = par2._solve_inverse_matrix(matrix)

            maybe_e_matrix = par2._mul_matrixes(matrix, inverse_matrix)

            e_matrix = par2._make_e_matrix()
            self.assertEqual(e_matrix, maybe_e_matrix)

    def test_archive_p4(self):
        # make par2 archive
        for bits in (4, 8, 16):
            redundancy = 15
            archive = Par2Archive(bits, redundancy)
            self.assertEqual(list, type(archive.slots))
            self.assertEqual(redundancy * 2, len(archive.slots))

            # make test data
            # >>> int.from_bytes(b'\x01\x0f', 'big')
            # 271 for bits is 4
            tmp = [int.to_bytes(x % 16, 1, 'big') for x in range(0, 271)]
            data = b''.join(tmp)
            data_size = len(data)
            parity_size, _, padding_size, _ = \
                archive.par2._calculate_size(data_size)

            # take data
            archive.take_data(data)
            # print('1 slots =', archive.slots)
            for i, p in enumerate(archive.slots[:redundancy]):
                self.assertEqual(bytes, type(p))
                self.assertEqual(parity_size, len(p),
                    'i = {}, p = {}'.format(i, p))
            for i, p in enumerate(archive.slots[redundancy:]):
                self.assertEqual(None, p, 'i = {}, p = {}'.format(i, p))

            # make parities
            archive.make_parities()
            for i, p in enumerate(archive.slots):
                self.assertEqual(bytes, type(p))
                self.assertEqual(parity_size, len(p),
                    'i = {}, p = {}'.format(i, p))

            archive._assign_nones(
                (0, 1, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14), #part
                (0, 1, 2)) #parity

            # fix up
            self.assertTrue(archive.enable_decode())
            raw_data = archive.fix_up_data()

            # make expected data for assert
            expected_decode_data = [None] * redundancy
            data_size_bytes = \
                int.to_bytes(data_size, TAIL_SIZE, 'big')
            pad = b'\x00' * padding_size
            data_pad = (data + pad + data_size_bytes)
            for i in range(redundancy):
                index = i * parity_size
                expected_decode_data[i] = data_pad[index:index+parity_size]
            self.assertDecodeData(archive.par2, expected_decode_data, \
                                    archive.decode_data, data_size)

            self.assertEqual(data_size, len(raw_data))
            self.assertEqual(data, raw_data)

    def test_archive_p4_lack_of_slots(self):
        # make par2 archive
        bits, redundancy = 4, 15
        archive = Par2Archive(bits, redundancy)

        # make test data
        # >>> int.from_bytes(b'\x01\x0f', 'big')
        # 271 for bits is 4
        tmp = [int.to_bytes(x % 16, 1, 'big') for x in range(0, 271)]
        data = b''.join(tmp)
        data_size = len(data)
        parity_size, _, padding_size, _= archive.par2._calculate_size(data_size)

        # take data
        archive.take_data(data)

        # make parities
        archive.make_parities()

        archive._assign_nones(
            (0, 1, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14), #part
            (0, 1, 2, 3)) #parity

        self.assertFalse(archive.enable_decode())
        with self.assertRaises(Par2Error) as raiz:
            # fix up
            raw_data = archive.fix_up_data()
        args = raiz.exception.args
        self.assertEqual(args[0], ('fix_up_data() cannot run. '
                                   'Because it does not have enough slots.'))

    def assertDecodeData(self, par2, edd, decode_data, data_size):
        self.assertEqual(list, type(decode_data))
        self.assertEqual(par2.redundancy, len(decode_data))
        parity_size, snip_size, padding_size, encode_size = \
                    par2._calculate_size(data_size)
        # print('decode_data', decode_data)
        for i in range(par2.redundancy):
            dd = decode_data[i]
            self.assertEqual(bytes, type(dd))
            self.assertEqual(parity_size, len(dd))
            self.assertEqual(edd[i], dd)

if __name__ == '__main__':
    unittest.main()