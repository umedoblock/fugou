import struct

import par2
from par2.const import *

__all__ = [ \
    'Par2Archive', 'Par2ArchiveError'
]

class Par2ArchiveError(BaseException):
    pass

class Par2Archive:
    def __init__(self, bits, redundancy=0, data=None, data_size=None):
        '''
        [part, part, .., parity, parity, ...]
        '''
        self.par2 = par2.Par2(bits, redundancy)
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
            raise Par2ArchiveError(('fix_up_data() cannot run. '
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
