# Copyright 2011 梅濁酒(umedoblock)

import math
import copy
import struct
import pprint
from sys import modules
import os
import pickle
import hashlib

try:
    import _par2
    from _par2 import _Par2
    from _par2 import Par2Error

except ImportError as e:
    print('cannot import _Par2')
    print('reason: ', e.args[0])
    class Par2Error(BaseException):
        pass

from par2.const import *
from par2.archive import *
from par2.abst import Par2_abstract
from par2.util import *

__all__ = [ \
    'Par2', 'Par2Error', \
    'Par2Archive', 'Par2ArchiveError'
]

def have_par2_c_extension():
    return '_par2' in modules

pp = pprint.PrettyPrinter(indent=4)

class ReedSolomon(object):

    @classmethod
    def get_reed_solomon(cls, bits):
        return cls.rds[bits]

    @classmethod
    def BigBang(cls):
        print('BigBang()')

        if hasattr(cls, 'rds'):
            return
        cls.rds = {}

      # /* bits      poly  octets   code_size */
        dokkaan = (
            ( 4,       19,      1,          2),
            ( 8,      285,      1,          2),
            (16,    65581,      2,          2),
#           (24, 16777243,      3,          4),
        )

        for bits, poly, octets, code_size in dokkaan:
            rds = ReedSolomon(bits, poly, octets, code_size)
            cls.rds[bits] = rds
        print('BigBang() done.')

    def __init__(self, bits, poly, octets, code_size):
        self._init_attr(bits, poly, octets, code_size)
        self._make_gf_and_gfi()

    def _init_attr(self, bits, poly, octets, code_size):
        self.bits = bits
        self.poly = poly
        self.octets = octets
        self.code_size = code_size

        self.w = 1 << self.bits
        self.gf_max = self.w - 1
        self.gf_size = self.w * self.code_size

    def _make_gf_and_gfi(self):
        path = 'gf_gfi_{}bits.pickle'.format(self.bits)
        if os.path.exists(path):
            print('os.path.exists({}) is True.'.format(path))
            sha1sums = {
                 4: 'dd8147807c230054c36539e0b2bf2699f2f94c79',
                 8: '287e3e18d461315911960e4897bad23e69f893b2',
                16: 'bf2c30e9b85800dd37c7501c9468904ec3bac5fa',
                24: 'fcae9c673f64584a34360daaeb18c53b61dc1f05',
            }

            with open(path, 'rb') as f:
                data = f.read()
            if sha1sums[self.bits] != hashlib.sha1(data).hexdigest():
                raise Par2Error('bits={}, sha1sum mismatched.', self.bits)

            gf_gfi = pickle.loads(data)
            self.gf = gf_gfi['gf']
            self.gfi = gf_gfi['gfi']

            return

        self.gf = [None] * self.w
        self.gfi = [None] * self.w

        bit_pattern = 1
        for i in range(self.gf_max):
            if bit_pattern & self.w:
                bit_pattern ^= self.poly
            self.gf[bit_pattern] = i
            self.gfi[i] = bit_pattern
            bit_pattern <<= 1

        if not os.path.exists(path):
            print('os.path.exists({}) is False.'.format(path))
            with open(path, 'wb') as f:
                gf_gfi = {}
                gf_gfi['gf'] = self.gf
                gf_gfi['gfi'] = self.gfi
                pickle.dump(gf_gfi, f, pickle.HIGHEST_PROTOCOL)

    def _mul(self, a, b):
        if a == 0 or b == 0:
            return 0
      # print('a =', a, 'b =', b)
        c = self.gf[a] + self.gf[b]
        if c < self.gf_max:
            return self.gfi[c]
        return self.gfi[c - self.gf_max]

    def _div(self, a, b):
        if a == 0:
            return 0
        c = self.gf[a] - self.gf[b]
        if c >= 0:
            return self.gfi[c]
        return self.gfi[c + self.gf_max]

    def _add(self, a, b):
        return a ^ b

    def _pow(self, a, x):
        if x == 0 or a == 1:
            return 1
        ret = a
        for i in range(x - 1):
            ret = self._mul(ret, a)
        return ret

class Par2_:
    C_EXTENSION = False

    if not have_par2_c_extension():
        ReedSolomon.BigBang()

    def _allocate_memory(self):
        self.rds = ReedSolomon.get_reed_solomon(self.bits)

    def _encode(self, parity_slots, data_slots, symbol_num):
        division = self.division
        octets = self.octets
        vector = self._make_vector()
        parity = self._make_vector()
        for i in range(symbol_num):
            for j in range(division):
                num_bytes = data_slots[j][i*octets:(i+1)*octets]
                num = int.from_bytes(num_bytes, 'big')
                vector[j] = num
            self._mul_matrix_vector(parity, self.vandermonde_matrix, vector)
            for j in range(division):
                num = parity[j]
                parity_slots[j][i*octets:(i+1)*octets] = \
                    int.to_bytes(num, octets, 'big')
        return parity_slots

    def _decode(self, decode_data, \
                merged_slots, inverse_matrix, symbol_num):
        octets = self.octets
        vector = self._make_vector()
        vertical_data = self._make_vector()

        for i in range(symbol_num):
            for j in range(self.division):
                num_bytes = merged_slots[j][i*octets:(i+1)*octets]
                num = int.from_bytes(num_bytes, 'big')
                vector[j] = num

            self._mul_matrix_vector(vertical_data, inverse_matrix, vector)
            for j in range(self.division):
                num = vertical_data[j]
                decode_data[j][i*octets:(i+1)*octets] = \
                    int.to_bytes(num, octets, 'big')

    def _mul_matrix_vector(self, answer, matrix, pari):
        for j in range(self.division):
            part = 0
            for i in range(self.division):
              # print('_mul_matrix_vector() =', matrix[j], pari[i])
                tmp = self.rds._mul(matrix[j][i], pari[i])
              # print('_add({}, {})'.format(part, tmp))
                part = self.rds._add(part, tmp)
            answer[j] = part

    def _solve_inverse_matrix(self, matrix):
        matrix = copy.deepcopy(matrix)
        im = inverse_matrix = self._make_e_matrix()

      # self._at_a_glance(matrix, im)
      # print()
        for k in range(self.division):
            # self._at_a_glance(matrix, im)
            if not matrix[k][k]:
                swap = False
                for j in range(k + 1, self.division):
                    if matrix[j][k]:
                        matrix[k], matrix[j] = matrix[j], matrix[k]
                        im[k], im[j] = im[j], im[k]
                        swap = True
                        break
                if not swap:
                    message = \
                        ('cannot make inverse_matrix. bits = {}, '
                         'division = {}.').format(self.bits, self.division)
                    raise Par2Error(message)

            if matrix[k][k] != 1:
                tmp = matrix[k][k]
                for i in range(self.division):
                    matrix[k][i] = self.rds._div(matrix[k][i], tmp)
                    im[k][i] = self.rds._div(im[k][i], tmp)

            for j in range(k + 1, self.division):
                foo = matrix[j][k]
                if not foo:
                    continue
                for i in range(self.division):
                    tmp1 = matrix[k][i]
                    tmp2 = self.rds._mul(foo, tmp1)
                    tmp3 = matrix[j][i]
                    matrix[j][i] = self.rds._add(tmp3, tmp2)

                    im1 = im[k][i]
                    im2 = self.rds._mul(foo, im1)
                    im3 = im[j][i]
                    im[j][i] = self.rds._add(im3, im2)
      # print('前進完了') # moving front done

        for k in range(self.division - 1):
            for j in range(self.division - 1 - k):
                z = self.division - 1 - k
                x = self.division - 1 - k
                y = self.division - 1 - k - j - 1
                foo = matrix[y][x]

                for i in range(self.division):
              #     tmp1 = matrix[z][i]
              #     tmp2 = self._mul(foo, tmp1)
              #     tmp3 = matrix[y][i]
              #     matrix[y][i] = self._add(tmp3, tmp2)

                    im1 = im[z][i]
                    im2 = self.rds._mul(foo, im1)
                    im3 = im[y][i]
                    im[y][i] = self.rds._add(im3, im2)
      # print()

        return inverse_matrix

    def _mul_matrixes(self, a, b):
        answer = self._make_square_matrix()
        for j in range(self.division):
            # print('koko 2')
            for i in range(self.division):
                tmp = 0
                # bb = [b[h][i] for h in range(self.division)]
                for k in range(self.division):
                    muled = self.rds._mul(a[j][k], b[k][i])
                    tmp = self.rds._add(tmp, muled)
                answer[j][i] = tmp
        return answer

    def _merge_slots(self, data_slots, parity_slots):
        merged_slots = [None] * self.division
        merged_matrix = self._make_e_matrix()
        j = 0

        vandermonde_matrix = self.vandermonde_matrix
      # print('vandermonde_matrix =')
      # pp.pprint(vandermonde_matrix)
        for i in range(self.division):
            if data_slots[i]:
                merged_slots[i] = data_slots[i]
            else:
                while not parity_slots[j]:
                    j += 1
                merged_matrix[i] = vandermonde_matrix[j]
                merged_slots[i] = parity_slots[j]
                j += 1
      # print('merged_slots =')
      # pp.pprint(merged_slots)
      # print('merged_matrix =')
      # pp.pprint(merged_matrix)
        return merged_slots, merged_matrix

    def _make_square_matrix(self, value=None):
        square_matrix = [None] * self.division
        for i in range(self.division):
            square_matrix[i] = [value] * self.division
        return square_matrix

    def _make_e_matrix(self):
        e = self._make_square_matrix(0)
        for i in range(self.division):
            e[i][i] = 1
      # self.view_matrix(e)
        return e

    def _make_vandermonde_matrix(self):
        vandermonde_matrix = [None] * self.division
        vm = vandermonde_matrix
        vm[0] = [1] * self.division
        for j in range(1, self.division):
            vm[j] = [None] * self.division
            for i in range(self.division):
                vm[j][i] = self.rds._mul(vm[j-1][i], i + 1)
        self.vandermonde_matrix = vm
      # self.view_matrix(vm)

class Par2MixIn:

    def _init_self(self, bits, division):
      # refs #22 and galois_{4,8,16,24}bits.log
      # poly = {4: 25, 8: 361, 16: 87749, 24: 16777435}
        poly = {4: 19, 8: 285, 16: 65581, 24: 16777243}
        if not bits in poly:
            raise Par2Error('must chose 4, 8, 16 or 24 for bits.')

        self.poly = poly[bits]
        self.bits = bits
        self.w = 1 << self.bits
        self.gf_max = self.w - 1
        self.digits = int(math.log(self.gf_max, 10)) + 1

        self.division = division
        if self.division < 2 or self.division > MAX_REDUNDANCY or \
           self.division > self.gf_max:
            if self.bits in (4, 8):
                max_division = self.gf_max
            else:
                max_division = MAX_REDUNDANCY
            message = 'division(={}) must be '.format(division)
            message += '2 <= division <= {}.'.format(max_division)
            raise Par2Error(message)
        octets = {4: 1, 8: 1, 16: 2, 24: 3}
        self.octets = octets[bits]
        self.vertical_size = self.division * self.octets
        code_size_ = {4: 2, 8: 2, 16: 2, 24: 4}
        self.code_size = code_size_[self.bits]
        self.horizontal_size = self.code_size * self.division

    def __init__(self, bits, division):
        self._init_self(bits, division)
        self._allocate_memory()

        self._make_vandermonde_matrix()

    def encode(self, data_slots=None, data=b''):
        if not data_slots:
            if not data:
                raise ValueError('must set data_slots or data.')
            else:
                data_slots = self._make_data_slots(data)
        slot_size = \
            min([len(slot) for slot in data_slots if slot and len(slot)])
        if not slot_size:
            raise()

        parity_slots = self._make_part_or_parity_slots(slot_size)
        symbol_num = slot_size // self.octets

        self._encode(parity_slots, data_slots, symbol_num)
        parity_slots = self._bytearray2bytes_all(parity_slots)

        return parity_slots

    def _make_data_slots(self, data, data_size=0):
        if not isinstance(data, bytes):
            raise ValueError('data must be bytes.')
        if not data_size:
            data_size = len(data)
        if not 1 <= data_size <= DATA_SIZE_MAX:
            msg = 'data_size must be 1 <= data_size <= 2 ** {} - 1'
            raise ValueError(msg.format(TAIL_SIZE * 8))
        self.data_size = data_size
        self._set_size(data_size)
        tail_bytes = int.to_bytes(data_size, self.octets, 'big')
        self.encode_data = \
            data + self._padding(self.padding_size) + tail_bytes
        sp = self.par2.split(self.encode_data, self.slot_size)
        self.slots[:self.division] = sp
        return sp

    def decode(self, slots):
        slot_size = min([len(slot) for slot in slots if slot and len(slot)])
        if not slot_size:
            raise()

        data_slots = slots[:self.division]
        parity_slots = slots[self.division:]
        merged_slots, merged_matrix = \
            self._merge_slots(data_slots, parity_slots)

        inverse_matrix = self._solve_inverse_matrix(merged_matrix)
        symbol_num = slot_size // self.octets
        decode_data = self._make_part_or_parity_slots(slot_size)

        self._decode(decode_data, merged_slots, inverse_matrix, symbol_num)
        decode_data = self._bytearray2bytes_all(decode_data)

        return decode_data

    def cat(self, decode_data):
        semi_raw_data = b''.join(decode_data)
        tail_bytes = semi_raw_data[-TAIL_SIZE:]
        data_size = int.from_bytes(tail_bytes, 'big')
      # print('data_size =', data_size, 'in cat()')
        raw_data = semi_raw_data[:data_size]
        return raw_data

    def split(self, encode_data, slot_size):
        sp = []
        for i in range(self.division):
            sp.append(encode_data[i*slot_size:(i+1)*slot_size])
        return sp

    def view_matrix(self, matrix):
        if self.bits >= 8:
            return
        fmt = ('{:' + str(self.digits + 1) + 'x}') * self.division
        for i in range(self.division):
            message = fmt.format(*matrix[i])
            print(message)
        print()

    def _calculate_size(self, data_size):
        if not 1 <= data_size <= DATA_SIZE_MAX:
            ValueError('data_size must be 1 <= data_size <= 2 ** 64 - 1')
        snip_size = data_size % self.vertical_size
        if not snip_size:
            padding_size = 0
        else:
            padding_size = self.vertical_size - snip_size
        encode_size = data_size + padding_size
      # vertical_size = division * octets
      # modulus = vertical_size (mod division)
        modulus = encode_size % self.division
        if modulus:
            raise ValueError('encode_size % self.division ='.format(encode_size % self.division))
        slot_size = encode_size // self.division
      # print('division = {}'.format(self.division))
      # print('data_size = {}'.format(data_size))
      # print('slot_size = {}'.format(slot_size))
      # print('snip_size = {}'.format(snip_size))
      # print('padding_size = {}'.format(padding_size))
      # print('encode_size = {}'.format(encode_size))
      # print('in _calculate_size()')
        return slot_size, snip_size, padding_size, encode_size

    def _div(self, a, b):
        if b == 0:
            raise ZeroDivisionError('tried to devide by zero')

        return super()._div(a, b)

    def _pow(self, a, x):
        if a == 0:
            raise ValueError('cannot accept argment a is zero')

        return super()._pow(a, x)

    def _make_part_or_parity_slots(self, slot_size):
        part_or_parity_slots = [None] * self.division
        for i in range(self.division):
            part_or_parity_slots[i] = bytearray(b'\x00' * slot_size)
        return part_or_parity_slots

    def _make_vector(self):
        return [None] * self.division

    def _bytearray2bytes_all(self, _bytearray):
        _bytes = _bytearray
        for j in range(len(_bytearray)):
            _bytes[j] = bytes(_bytearray[j])
        return _bytes

    def _at_a_glance(self, matrix, inverse_matrix, stop=False):
        im = inverse_matrix
        print('matrix =')
        self.view_matrix(matrix)
        print('inverse_matrix =')
        self.view_matrix(im)
        if stop:
            input()

if have_par2_c_extension():
    print("have par2 c extension.")
    class Par2(Par2MixIn, _Par2, Par2_abstract): pass
else:
    print("don't have par2 c extension.")
    class Par2(Par2MixIn, Par2_, Par2_abstract): pass

if __name__ == '__main__':
    division = 15
    data = b'\x00' * 3855 # 0xf0f

    archive = Par2Archive(4, division)
    archive.take_data(data)
    # make parities
    archive.make_parities()

    # delete all parts
    archive.slots[:archive.par2.division] = [None] * archive.par2.division

    # fix up
    raw_data = archive.fix_up_data()

    print('data == raw_data is', data == raw_data)
    archive.clear()
