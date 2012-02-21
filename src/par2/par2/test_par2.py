# Copyright 2011 梅どぶろく(umedoblock)

import gc
import unittest
import pprint
import sys, os
dname = os.path.dirname(os.path.abspath(__file__))
# print('dname =', dname)
libdir = os.path.join(dname, '..')
# print('libdir =', libdir)
sys.path.insert(0, libdir)

pp = pprint.PrettyPrinter(indent=4)

from par2 import *
from par2.util import *
from par2.const import *

class _TestPar2(unittest.TestCase):

    def test_bits_and_redundancy(self):
        pass
        p4 = Par2(4, 2)
        self.assertEqual(2, p4.redundancy)
        p4 = Par2(4, 10)
        self.assertEqual(10, p4.redundancy)
        p4 = Par2(4, (1 << 4) - 1)
        self.assertEqual((1 << 4) - 1, p4.redundancy)

        p8 = Par2(8, 2)
        self.assertEqual(2, p8.redundancy)
        p8 = Par2(8, 122)
        self.assertEqual(122, p8.redundancy)
        p8 = Par2(8, (1 << 8) - 1)
        self.assertEqual((1 << 8) - 1, p8.redundancy)

        if Par2.C_EXTENSION:
            p16 = Par2(16, 2)
            self.assertEqual(2, p16.redundancy)
            p16 = Par2(16, 1000)
            self.assertEqual(1000, p16.redundancy)
            p16 = Par2(16, MAX_REDUNDANCY)
            self.assertEqual(MAX_REDUNDANCY, p16.redundancy)

            p24 = Par2(24, 2)
            self.assertEqual(2, p24.redundancy)
            p24 = Par2(24, 1234)
            self.assertEqual(1234, p24.redundancy)
            p24 = Par2(24, MAX_REDUNDANCY)
            self.assertEqual(MAX_REDUNDANCY, p24.redundancy)

    def test_exception(self):
        with self.assertRaises(KeyError):
            Par2(5, 10)

        redundancies = (-1, 0, 1, 1 << 4)
        for redundancy in redundancies:
            with self.assertRaises(Par2Error) as raiz:
                Par2(4, redundancy)
            args = raiz.exception.args
            self.assertEqual(('redundancy must be '
                              '2 <= redundancy <= 15'), args[0])

        redundancies = (-1, 0, 1, 1 << 8)
        for redundancy in redundancies:
            with self.assertRaises(Par2Error) as raiz:
                Par2(8, redundancy)
            args = raiz.exception.args
            self.assertEqual(('redundancy must be '
                              '2 <= redundancy <= 255'), args[0])

        redundancies = (-1, 0, 1, MAX_REDUNDANCY + 1, 1 << 16)
        for redundancy in redundancies:
            with self.assertRaises(Par2Error) as raiz:
                Par2(16, redundancy)
            args = raiz.exception.args
            self.assertEqual(('redundancy must be '
                              '2 <= redundancy <= 8192'), args[0])

        redundancies = (-1, 0, 1, MAX_REDUNDANCY + 1, 1 << 24)
        for redundancy in redundancies:
            with self.assertRaises(Par2Error) as raiz:
                Par2(24, redundancy)
            args = raiz.exception.args
            self.assertEqual(('redundancy must be '
                              '2 <= redundancy <= 8192'), args[0])

    def test_mul_and_div(self):
        bitss = (4,)
        bitss = (4, 8)
        bitss = (4, 8, 16, 24)
        bitss = (4, 8, 16)
        dic = {\
            4: {'redundancy': 10, 'w': None, 'gf_max': None},
            8: {'redundancy': 100, 'w': None, 'gf_max': None},
            16: {'redundancy': 50, 'w': 50, 'gf_max': 50},
            24: {'redundancy': 50, 'w': 50, 'gf_max': 50}
        }
        count = 0
        for bits, attrs in dic.items():
            redundancy = attrs['redundancy']
          # print('bits =', bits)
            par2 = Par2(bits, redundancy)
          # print('gf =')
          # _gf = bytes_to_matrix(par2.gf, \
          #                       par2.redundancy, par2.horizontal_size,
          #                       par2.code_size)
          # pp.pprint(_gf)
          # print('gfi =')
          # _gfi = bytes_to_matrix(par2.gfi, \
          #                       par2.redundancy, par2.horizontal_size,
          #                       par2.code_size)
          # pp.pprint(_gfi)
            w = attrs['w'] or par2.w
            gf_max = attrs['gf_max'] or par2.gf_max
            for a in range(w):
                for n in range(gf_max):
                    b = n + 1
                    mul = par2._mul(a, b)
                    c = par2._div(mul, b)
                  # print('a, c =', a, c)
                  # if count == 68070:
                  #     print('count =', count)
                  #     print(a, b, mul, c)
                    self.assertEqual(a, c)
                  # count += 1

    def test__pow(self):
        redundancies = {4: 4, 8: 4}
        redundancies = {4: 15, 8: 50, 16: 50}
        redundancies = {24: 50}
        redundancies = {4: 15, 8: 50, 16: 50, 24: 50}
        for bit, redundancy in redundancies.items():
            par2 = Par2(bit, redundancy)
            vm = [None] * par2.redundancy
          # print('bits =', bit, 'redundancy =', redundancy, \
          #       'code_size =', par2.code_size)

            for j in range(par2.redundancy):
                vm[j] = [None] * par2.redundancy
                for i in range(par2.redundancy):
                    vm[j][i] = par2._pow(i + 1, j)
            if Par2.C_EXTENSION:
                _vm = par2._get_vandermonde_matrix()
        #       print('_vm =', _vm)
                _vm = bytes_to_matrix(_vm, \
                                      par2.redundancy, par2.horizontal_size,
                                      par2.code_size)
                vandermonde_matrix = _vm
            else:
                vandermonde_matrix = par2.vandermonde_matrix

          # print('vm =')
          # pp.pprint(vm)
          # print('vandermonde_matrix =')
          # pp.pprint(vandermonde_matrix)

            self.assertEqual(vandermonde_matrix, vm)

    def test__mul_matrix_by_e_matrix(self):
        redundancies = {4: 15, 8: 50, 16: 50}
        redundancies = {24: 50}
        redundancies = {4: 15, 8: 50, 16: 50, 24: 50}
        for bits, redundancy in redundancies.items():
            par2 = Par2(bits, redundancy)
            e_matrix = par2._make_e_matrix()

            matrix = []
            for i in range(par2.redundancy):
                foo = [x % par2.w for x in range(i, par2.redundancy + i)]
                matrix.append(foo)
          # print('matrix =')
          # pp = pprint.PrettyPrinter(indent=4)
          # pp.pprint(matrix)

            if Par2.C_EXTENSION:
                matrix = matrix_to_bytes(matrix, par2.code_size)
          # print('len(matrix) =', len(matrix))

            muled_matrix = par2._mul_matrixes(matrix, e_matrix)
          # print('len(muled_matrix) =', len(muled_matrix))
            self.assertEqual(matrix, muled_matrix)

            muled_matrix = par2._mul_matrixes(e_matrix, matrix)
            self.assertEqual(matrix, muled_matrix)

    def test___matrix_to_bytes_bytes_to_matrix(self):
        pp = pprint.PrettyPrinter(indent=4)

        redundancies = {8: 5}
        redundancies = {16: 5}
        redundancies = {24: 4}
        redundancies = {4: 10, 8:10, 16:10}
        redundancies = {4: 4}
        redundancies = {4: 15, 8: 50, 16: 50}
        redundancies = {24: 10}
        redundancies = {4: 15, 8:50, 16:50, 24: 50}
        for bits, redundancy in redundancies.items():
            par2 = Par2(bits, redundancy)

          # print('bits = {}, redundancy = {}'.format(bits, redundancy))
            if par2.C_EXTENSION:
                _vm = par2._get_vandermonde_matrix()
                _vm = bytes_to_matrix(_vm, \
                                      par2.redundancy, par2.horizontal_size,
                                      par2.code_size)
                matrix = _vm
            else:
                matrix = par2.vandermonde_matrix
          # print('matrix =')
          # pp.pprint(matrix)
            by = matrix_to_bytes(matrix, par2.code_size)
          # print('bytes =')
          # pp.pprint(by)
            mt = bytes_to_matrix(by, par2.redundancy, par2.horizontal_size, \
                                     par2.code_size)
          # print('mt =')
          # pp.pprint(mt)
            self.assertEqual(matrix, mt)

            by2 = matrix_to_bytes(mt, par2.code_size)
            self.assertEqual(by, by2)

    def test__solve_inverse_matrix(self):
        pp = pprint.PrettyPrinter(indent=4)

        redundancies = {4:15, 8:50, 16:50}
        redundancies = {24: 10}
        redundancies = {4: 15}
        redundancies = {8: 5}
        redundancies = {16: 5}
        redundancies = {24: 50}
        redundancies = {4: 15, 8: 50, 16: 50}
        redundancies = {4: 15, 8: 50, 16: 50, 24: 50}
        for bits, redundancy in redundancies.items():
            par2 = Par2(bits, redundancy)

            if Par2.C_EXTENSION:
                matrix = par2._get_vandermonde_matrix()
              # by = matrix
              # mt = bytes_to_matrix(by, par2.redundancy, par2.horizontal_size)
              # print('mt is')
              # pp.pprint(mt)

              # im = bytes_to_matrix(inverse_matrix, par2.redundancy, \
              #                      par2.horizontal_size)
              # print('inverse_matrix =')
              # pp.pprint(im)
            else:
                matrix = par2.vandermonde_matrix
              # print('matrix =')
              # pp.pprint(matrix)
                by = matrix_to_bytes(matrix, par2.code_size)
                mt = bytes_to_matrix( \
                        by, par2.redundancy, par2.horizontal_size, \
                        par2.code_size)
              # print('mt =')
              # pp.pprint(mt)
                self.assertEqual(matrix, mt)

            inverse_matrix = par2._solve_inverse_matrix(matrix)
            if Par2.C_EXTENSION:
                matrix = par2._get_vandermonde_matrix()
            maybe_e_matrix = par2._mul_matrixes(matrix, inverse_matrix)

            e_matrix = par2._make_e_matrix()
          # print('e_matrix =')
          # pp.pprint(e_matrix)

            self.assertEqual(e_matrix, maybe_e_matrix)

          # print('e_matrix =')
          # if Par2.C_EXTENSION:
          #     e_matrix = \
          #         bytes_to_matrix(e_matrix, \
          #                         par2.redundancy, par2.horizontal_size)
          # pp.pprint(e_matrix)

          # print('maybe_e_matrix =')
          # if Par2.C_EXTENSION:
          #     maybe_e_matrix = \
          #         bytes_to_matrix(maybe_e_matrix, \
          #                         par2.redundancy, par2.horizontal_size)
          # pp.pprint(maybe_e_matrix)

          # print('matrix =')
          # if Par2.C_EXTENSION:
          #     matrix = \
          #         bytes_to_matrix(matrix, \
          #                         par2.redundancy, par2.horizontal_size)
          # pp.pprint(matrix)

          # print('inverse_matrix =')
          # if Par2.C_EXTENSION:
          #     inverse_matrix = \
          #         bytes_to_matrix(inverse_matrix, \
          #                         par2.redundancy, par2.horizontal_size)
          # pp.pprint(inverse_matrix)

    def test_archive_p4(self):
        # make par2 archive
        bits = (4,)
        bits = (4, 8)
        bits = (4, 8, 16)
        bits = (4, 8, 16, 24)
        for bit in bits:
            redundancy = 15
            archive = Par2Archive(bit, redundancy)
            self.assertEqual(list, type(archive.slots))
            self.assertEqual(redundancy * 2, len(archive.slots))

            # make test data
            # >>> int.from_bytes(b'\x01\x0f', 'big')
            # 271 for bit is 4
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

          # print('archive.slots =')
          # pp.pprint(archive.slots)

            # make expected data for assert
            expected_decode_data = [None] * redundancy
            data_size_bytes = \
                int.to_bytes(data_size, 8, 'big')
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
        parity_size, _, padding_size, _= \
            archive.par2._calculate_size(data_size)

        # take data
        archive.take_data(data)

        # make parities
        archive.make_parities()

        archive._assign_nones(
            (0, 1, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14), #part
            (0, 1, 2, 3)) #parity

        self.assertFalse(archive.enable_decode())
        with self.assertRaises(Par2ArchiveError) as raiz:
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
  # gc.set_debug(gc.DEBUG_LEAK)
    unittest.main()
