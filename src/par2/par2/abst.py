from abc import ABCMeta, abstractmethod, abstractproperty
class Par2_abstract(metaclass=ABCMeta):

    @abstractmethod
    def _allocate_memory(self):
        pass

    @abstractmethod
    def _encode(self, parity_slots, data_slots, symbol_num):
        raise NotImplementedError()

    @abstractmethod
    def _decode(self, decode_data, \
                data_and_parity, inverse_matrix, symbol_num):
        raise NotImplementedError()

    @abstractmethod
    def _merge_slots(self, slots):
        raise NotImplementedError()

    @abstractmethod
    def _solve_inverse_matrix(self, matrix):
        raise NotImplementedError()
