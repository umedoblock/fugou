# Copyright 2011 梅どぶろく(umedoblock)

import math
import copy
import struct
import pprint

__all__ = [ \
    'Par2', 'Par2Archive', 'Par2Error', \
    'matrix_to_bytes', 'bytes_to_matrix' \
]

pp = pprint.PrettyPrinter(indent=4)

def matrix_to_bytes(matrix):
    elements = [None] * len(matrix)
    for i, m in enumerate(matrix):
        elements[i] = struct.pack('=' + 'H' * len(m), *m)
    return b''.join(elements)

def bytes_to_matrix(bys, redundancy, horizontal_size):
    matrix = [None] * redundancy
    for i in range(redundancy):
        horizontal_bytes = bys[i*horizontal_size:(i+1)*horizontal_size]
        vector = struct.unpack('=' + 'H' * redundancy, horizontal_bytes)
        matrix[i] = list(vector)

    return matrix

class Par2Error(BaseException):
    pass

TAIL_SIZE = 8
DATA_SIZE_MAX = (1 << (TAIL_SIZE * 8)) - 1

class Par2Archive:
    def __init__(self, bits, redundancy=0, data=None, data_size=None):
        '''
        [part, part, .., parity, parity, ...]
        '''
        self.par2 = Par2(bits, redundancy)
        self.redundancy = self.par2.redundancy
        self._init_attr()
        if data:
            self.take_data(data, data_size)

    def take_data(self, data, data_size=None):
        if not isinstance(data, bytes):
            raise ValueError('data must be bytes.')
        if not data_size:
            data_size = len(data)
        if not 1 <= data_size <= DATA_SIZE_MAX:
            msg = 'data_size must be 1 <= data_size <= 2 ** {} - 1'
            ValueError(msg.format(TAIL_SIZE * 8))
        self.data_size = data_size
      # print('data_size =', data_size, 'in take_data()')
        self._set_size(data_size)
        fmt = {4: '>I', 8: '>Q'}
        tail_bytes = struct.pack(fmt[TAIL_SIZE], data_size)
        self.encode_data = \
            data + self._padding(self.padding_size) + tail_bytes
        sp = self.par2.split(self.encode_data, self.slot_size)
        self.slots[:self.redundancy] = sp
        return sp

    def make_parities(self):
        parities = self.par2.encode(self.slots[:self.redundancy])
        self.slots[self.redundancy:] = parities
        return parities

    def fix_up_data(self):
        if self.enable_decode():
            # memo: slots[:redundancy] に入れる。
            self.decode_data = self.par2.decode(self.slots)
        else:
            raise Par2Error(('fix_up_data() cannot run. '
                             'Because it does not have enough slots.'))
        return self.par2.cat(self.decode_data)

    def get_part(self, index):
        self._check_part_or_parity(index)
        if index < 0:
            index += self.redundancy
        return self.slots[index]

    def get_parity(self, index):
        self._check_part_or_parity(index)
        return self.slots[self.redundancy + index]

    def set_part(self, index, part):
        self._check_part_or_parity(index, part)
        if index < 0:
            index += self.redundancy
        self.slots[index] = part

    def set_parity(self, index, parity):
        self._check_part_or_parity(index, parity)
        self.slots[self.redundancy + index] = parity

    def enable_slots(self):
        return [slot for slot in self.slots if slot]
      # 10 ** 6 p3 li1.py  2.10s user 0.00s system 99% cpu 2.111 total
      # 10 ** 7 p3 li1.py  21.70s user 0.02s system 99% cpu 21.741 total

      # return list(filter(None, self.slots))
      # 10 ** 6 p3 li1.py  2.46s user 0.02s system 99% cpu 2.485 total
      # 10 ** 7 p3 li1.py  24.16s user 0.04s system 99% cpu 24.222 total

    def enable_decode(self):
        if len(self.enable_slots()) >= self.redundancy:
            return True
        else:
            return False

    def clear(self):
        self._init_attr()

    def _init_attr(self):
        self.slots = [None] * (self.redundancy * 2)
        self.data_size = 0
        self.slot_size, self.snip_size, \
        self.padding_size, self.encode_size = [0] * 4
        self.encode_data = b''

    def _check_part_or_parity(self, index, part_or_parity=None):
        if part_or_parity and len(part_or_parity) < self.slot_size:
            raise ValueError('len(part or parity) < slot_size.')
        if not -self.redundancy <= index < self.redundancy:
            raise IndexError('index must be less than par2.redundancy.')

    def _padding(self, padding_size):
        return b'\x00' * padding_size

    def _set_size(self, data_size):
        self.slot_size, self.snip_size, \
        self.padding_size, self.encode_size = \
            self.par2._calculate_size(self.data_size)

    def _assign_nones(self, part_indexies, parity_indexies):
        for index in part_indexies:
            self.set_part(index, None)
        for index in parity_indexies:
            self.set_parity(index, None)

from abc import ABCMeta, abstractmethod, abstractproperty
class Par2_abstract(metaclass=ABCMeta):

    @abstractmethod
    def _encode(self, parity_slots, data_slots,
                 redundancy, symbol_num, octets):
        pass

    @abstractmethod
    def _pick_up_rows(self, slots):
        pass

    @abstractmethod
    def _mul_matrixes(self, matrix, inverse_matrix):
        pass

    @abstractmethod
    def _make_square_matrix(self, value=None):
        pass

    @abstractmethod
    def _make_gf_and_gfi(self):
        pass

    @abstractmethod
    def _make_vandermonde_matrix(self):
        pass

    @abstractmethod
    def _mul(self, a, b):
        pass

    @abstractmethod
    def _div(self, a, b):
        pass

    @abstractmethod
    def _add(self, a, b):
        pass

    @abstractmethod
    def _solve_inverse_matrix(self, matrix):
        pass

    @abstractmethod
    def _make_e_matrix(self):
        pass

class Par2_base(Par2_abstract):
    C_EXTENSION = False

    def _encode(self, parity_slots, data_slots,
                 redundancy, symbol_num, octets):
        vector = self._make_vector()
        parity = self._make_vector()
        for i in range(symbol_num):
            for j in range(redundancy):
                num_bytes = data_slots[j][i*octets:(i+1)*octets]
                num = struct.unpack(self.format, num_bytes)[0]
                vector[j] = num
            self._mul_matrix_vector(parity, self.vander_matrix, vector)
            for j in range(redundancy):
                parity_slots[j][i*octets:(i+1)*octets] = \
                    struct.pack(self.format, parity[j])
        return parity_slots

    def _solve_inverse_matrix(self, matrix):
        matrix = copy.deepcopy(matrix)
        im = inverse_matrix = self._make_e_matrix()

      # self._at_a_glance(matrix, im)
      # print()
        for k in range(self.redundancy):
            # self._at_a_glance(matrix, im)
            if not matrix[k][k]:
                swap = False
                for j in range(k + 1, self.redundancy):
                    if matrix[j][k]:
                        matrix[k], matrix[j] = matrix[j], matrix[k]
                        im[k], im[j] = im[j], im[k]
                        swap = True
                        break
                if not swap:
                    message = \
                        ('cannot make inverse_matrix. redundancy = {}, ',
                         'gf_max = {}').format(self.redundancy, self.gf_max)
                    raise Par2RankError(message)

            if matrix[k][k] != 1:
                tmp = matrix[k][k]
                for i in range(self.redundancy):
                    matrix[k][i] = self._div(matrix[k][i], tmp)
                    im[k][i] = self._div(im[k][i], tmp)

            for j in range(k + 1, self.redundancy):
                foo = matrix[j][k]
                if not foo:
                    continue
                for i in range(self.redundancy):
                    tmp1 = matrix[k][i]
                    tmp2 = self._mul(foo, tmp1)
                    tmp3 = matrix[j][i]
                    matrix[j][i] = self._add(tmp3, tmp2)

                    im1 = im[k][i]
                    im2 = self._mul(foo, im1)
                    im3 = im[j][i]
                    im[j][i] = self._add(im3, im2)
      # print('前進完了') # moving front done

        for k in range(self.redundancy - 1):
            for j in range(self.redundancy - 1 - k):
                z = self.redundancy - 1 - k
                x = self.redundancy - 1 - k
                y = self.redundancy - 1 - k - j - 1
                foo = matrix[y][x]

                for i in range(self.redundancy):
              #     tmp1 = matrix[z][i]
              #     tmp2 = self._mul(foo, tmp1)
              #     tmp3 = matrix[y][i]
              #     matrix[y][i] = self._add(tmp3, tmp2)

                    im1 = im[z][i]
                    im2 = self._mul(foo, im1)
                    im3 = im[y][i]
                    im[y][i] = self._add(im3, im2)
      # print()

        return inverse_matrix

    def _mul_matrixes(self, a, b):
        answer = self._make_square_matrix()
        for j in range(self.redundancy):
            # print('koko 2')
            for i in range(self.redundancy):
                tmp = 0
                # bb = [b[h][i] for h in range(self.redundancy)]
                for k in range(self.redundancy):
                    muled = self._mul(a[j][k], b[k][i])
                    tmp = self._add(tmp, muled)
                answer[j][i] = tmp
        return answer

    def _make_square_matrix(self, value=None):
        square_matrix = [None] * self.redundancy
        for i in range(self.redundancy):
            square_matrix[i] = [value] * self.redundancy
        return square_matrix

    def _make_e_matrix(self):
        e = self._make_square_matrix(0)
        for i in range(self.redundancy):
            e[i][i] = 1
      # self.view_matrix(e)
        return e

    def _make_gf_and_gfi(self):
        self.gf = [None] * self.w
        self.gfi = [None] * self.w

        bit_pattern = 1
        for i in range(self.gf_max):
            if bit_pattern & self.w:
                bit_pattern ^= self.poly
            self.gf[bit_pattern] = i
            self.gfi[i] = bit_pattern
            bit_pattern <<= 1

    def _make_vandermonde_matrix(self):
        vander_matrix = [None] * self.redundancy
        vm = vander_matrix
        vm[0] = [1] * self.redundancy
        for j in range(1, self.redundancy):
            vm[j] = [None] * self.redundancy
            for i in range(self.redundancy):
                vm[j][i] = self._mul(vm[j-1][i], i + 1)
        self.vander_matrix = vm
      # self.view_matrix(vm)

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

try:
#   raise ImportError('test')
    from _par2 import _Par2
    class Par2_base(_Par2, Par2_abstract):
        pass

except ImportError as e:
    print('cannot import _Par2')
    print('reason: ', e.args[0])

print('Par2_base is {}'.format(Par2_base))

class Par2(Par2_base):

    def __init__(self, bits, redundancy=0):
      # print('Par2.__init__()')
      # refs #22 and galois_{4,8,16}bits.log
      # poly = {4: 25, 8: 501, 16: 131053}
        poly = {4: 19, 8: 285, 16: 65581}
        try:
            self.poly = poly[bits]
        except KeyError:
            raise KeyError("must chose 4, 8 or 16 for bits.")
        self.bits = bits
        self.w = 1 << self.bits
        self.gf_max = self.w - 1
        self.digits = int(math.log(self.gf_max, 10)) + 2
        if not redundancy:
            self.redundancy = self.gf_max
        else:
            self.redundancy = redundancy
        if 2 <= self.redundancy <= self.gf_max:
            pass
        else:
            message = 'redundancy is {}. '.format(self.redundancy)
            message += 'redundancy must be '
            message += '2 <= redundancy <= {}'.format(self.gf_max)
            raise Par2Error(message)
        octets = {4: 1, 8: 1, 16: 2}
        self.octets = octets[bits]
        self.vertical_size = self.redundancy * self.octets

        su = super(Par2, self)
        su.__init__()

        if not Par2.C_EXTENSION:
            # 2 means sizeof(unsigned short)
            self.horizontal_size = 2 * self.redundancy
            fmt = {4: 'B', 8: 'B', 16: 'H'}
            self.format = '>{}'.format(fmt[bits])

        su._make_gf_and_gfi()
        su._make_vandermonde_matrix()

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

        self._encode(parity_slots, data_slots,
                     self.redundancy, symbol_num, self.octets)
        parity_slots = self._bytearray2bytes_all(parity_slots)

        return parity_slots

    def _make_data_slots(self, data, data_size=0):
        if not isinstance(data, bytes):
            raise ValueError('data must be bytes.')
        if not data_size:
            data_size = len(data)
        if not 1 <= data_size <= DATA_SIZE_MAX:
            msg = 'data_size must be 1 <= data_size <= 2 ** {} - 1'
            ValueError(msg.format(TAIL_SIZE * 8))
        self.data_size = data_size
        self._set_size(data_size)
        fmt = {4: '>I', 8: '>Q'}
        tail_bytes = struct.pack(fmt[TAIL_SIZE], data_size)
        self.encode_data = \
            data + self._padding(self.padding_size) + tail_bytes
        sp = self.par2.split(self.encode_data, self.slot_size)
        self.slots[:self.redundancy] = sp
        return sp

    def decode(self, slots):
        slot_size = min([len(slot) for slot in slots if slot and len(slot)])
        if not slot_size:
            raise()

        data_and_parity, matrix = self._pick_up_rows(slots)
      # print('matrix =')
      # pp.pprint(matrix)
        if Par2.C_EXTENSION:
            matrix = matrix_to_bytes(matrix)

        inverse_matrix = self._solve_inverse_matrix(matrix)
        vector = self._make_vector()
        symbol_num = slot_size // self.octets
        decode_data = self._make_part_or_parity_slots(slot_size)

        self._decode(decode_data, data_and_parity, inverse_matrix,
                     self.redundancy, symbol_num, self.octets)
        decode_data = self._bytearray2bytes_all(decode_data)

        return decode_data

    def cat(self, decode_data):
        semi_raw_data = b''.join(decode_data)
        tail_bytes = semi_raw_data[-TAIL_SIZE:]
        fmt = {4: '>I', 8: '>Q'}
        data_size = struct.unpack(fmt[TAIL_SIZE], tail_bytes)[0]
      # print('data_size =', data_size, 'in cat()')
        raw_data = semi_raw_data[:data_size]
        return raw_data

    def split(self, encode_data, slot_size):
        sp = []
        for i in range(self.redundancy):
            sp.append(encode_data[i*slot_size:(i+1)*slot_size])
        return sp

    def view_matrix(self, matrix):
        if self.bits >= 8:
            return
        fmt = ('{:' + str(self.digits) + 'x}') * self.redundancy
        for i in range(self.redundancy):
            message = fmt.format(*matrix[i])
            print(message)
        print()

    def _decode(self, decode_data, data_and_parity, inverse_matrix,
                 redundancy, symbol_num, octets):
        vector = self._make_vector()
        vertical_data = self._make_vector()
        if Par2.C_EXTENSION:
            inverse_matrix = bytes_to_matrix(inverse_matrix,
                                             self.redundancy,
                                             self.horizontal_size)

        for i in range(symbol_num):
            for j in range(self.redundancy):
                num_bytes = data_and_parity[j][i*octets:(i+1)*octets]
                if Par2.C_EXTENSION:
                    fmt = {4: 'B', 8: 'B', 16: 'H'}
                    format = '>{}'.format(fmt[self.bits])
                    num = struct.unpack(format, num_bytes)[0]
                else:
                    num = struct.unpack(self.format, num_bytes)[0]
                vector[j] = num
            self._mul_matrix_vector(vertical_data, inverse_matrix, vector)
            for j in range(self.redundancy):
                if Par2.C_EXTENSION:
                    fmt = {4: 'B', 8: 'B', 16: 'H'}
                    format = '>{}'.format(fmt[self.bits])
                    decode_data[j][i*octets:(i+1)*octets] = \
                        struct.pack(format, vertical_data[j])
                else:
                    decode_data[j][i*octets:(i+1)*octets] = \
                        struct.pack(self.format, vertical_data[j])

            if Par2.C_EXTENSION:
                pass
          #     matrix = par2._get_vandermonde_matrix()
          #     bytes_to_matrix(matrix, \
          #                     par2.redundancy, par2.horizontal_size)

    def _pick_up_rows(self, slots):
        data_and_parity = [None] * self.redundancy
        matrix = self._make_e_matrix()
        j = 0
        if Par2.C_EXTENSION:
            vander_matrix = self._get_vandermonde_matrix()
            vander_matrix = bytes_to_matrix(vander_matrix, \
                                            self.redundancy, \
                                            self.horizontal_size)
            matrix = bytes_to_matrix(matrix, \
                                     self.redundancy, \
                                     self.horizontal_size)
        else:
            vander_matrix = self.vander_matrix
      # print('vander_matrix =')
      # pp.pprint(vander_matrix)
        for i in range(self.redundancy):
            if slots[i]:
                data_and_parity[i] = slots[i]
            else:
                while not slots[self.redundancy + j]:
                    j += 1
                matrix[i] = vander_matrix[j]
                data_and_parity[i] = slots[self.redundancy + j]
                j += 1
        return data_and_parity, matrix

# matrix_to_bytes(matrix):
# bytes_to_matrix(bys, redundancy, horizontal_size):

    def _mul_matrix_vector(self, answer, matrix, pari):
        for j in range(self.redundancy):
            part = 0
            for i in range(self.redundancy):
              # print('_mul_matrix_vector() =', matrix[j], pari[i])
                tmp = self._mul(matrix[j][i], pari[i])
                part = self._add(part, tmp)
            answer[j] = part

    def _calculate_size(self, data_size):
        if not 1 <= data_size <= DATA_SIZE_MAX:
            ValueError('data_size must be 1 <= data_size <= 2 ** 64 - 1')
        snip_size = (data_size + TAIL_SIZE) % self.vertical_size
        if not snip_size:
            padding_size = 0
        else:
            padding_size = self.vertical_size - snip_size
        encode_size = data_size + padding_size + TAIL_SIZE
        if encode_size % self.redundancy:
            raise ValueError('encode_size % self.redundancy ='.format(encode_size % self.redundancy))
        slot_size = encode_size // self.redundancy
      # print('redundancy = {}'.format(self.redundancy))
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
            raise RuntimeError('cannot accept argment a is zero')
        if x == 0 or a == 1:
            return 1
        ret = a
        for i in range(x):
            ret = self._mul(ret, a)
        return ret

    def _make_part_or_parity_slots(self, slot_size):
        part_or_parity_slots = [None] * self.redundancy
        for i in range(self.redundancy):
            part_or_parity_slots[i] = bytearray(b'\x00' * slot_size)
        return part_or_parity_slots

    def _make_vector(self):
        return [None] * self.redundancy

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

if __name__ == '__main__':
    redundancy = 15
    data = b'\x00' * 3855 # 0xf0f

    archive = Par2Archive(4, redundancy)
    archive.take_data(data)
    # make parities
    archive.make_parities()

    # delete all parts
    archive.slots[:archive.par2.redundancy] = [None] * archive.par2.redundancy

    # fix up
    raw_data = archive.fix_up_data()

    print('data == raw_data is', data == raw_data)
    archive.clear()
