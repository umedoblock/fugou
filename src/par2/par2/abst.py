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
    def _mul_matrixes(self, matrix, inverse_matrix):
        raise NotImplementedError()

    @abstractmethod
    def _make_square_matrix(self, value=None):
        raise NotImplementedError()

    @abstractmethod
    def _make_gf_and_gfi(self):
        raise NotImplementedError()

    @abstractmethod
    def _make_vandermonde_matrix(self):
        raise NotImplementedError()

    @abstractmethod
    def _mul(self, a, b):
        raise NotImplementedError()

    @abstractmethod
    def _div(self, a, b):
        raise NotImplementedError()

    @abstractmethod
    def _pow(self, a, x):
        raise NotImplementedError()

    @abstractmethod
    def _add(self, a, b):
        raise NotImplementedError()

    @abstractmethod
    def _solve_inverse_matrix(self, matrix):
        raise NotImplementedError()

    @abstractmethod
    def _make_e_matrix(self):
        raise NotImplementedError()
