from abc import ABCMeta, abstractmethod, abstractproperty

class Bar_abstract(metaclass=ABCMeta):
    __doc__ = 'Bar_abstract.__doc__'
    def __init__(self, n=8888):
        print('Bar_abstract.__init__()')
        print('Bar_abstract.__mro__ =', Bar_abstract.__mro__)
        print('self.__dict__ =', self.__dict__)
        print('id(self.__dict__) =', id(self.__dict__))
        s = super()
        print('s =', s)
        print('s.__thisclass__ =', s.__thisclass__)
        print('s.__self__ =', s.__self__)
        print('s.__self_class__ =', s.__self_class__)
    #   self.ss = 'abc'
        self.num = n

    @abstractmethod
    def abs_meth(self):
        pass

class Bar_base(Bar_abstract):
    CONST = 'PURE PYTHON'
    def __init__(self, n=100):
        print('Bar_base.__init__()')
        print('Bar_base.__mro__ =', Bar_base.__mro__)
        print('self.__dict__ =', self.__dict__)
        print('id(self.__dict__) =', id(self.__dict__))
        s = super()
        print('super().__init__() in Bar_base pure Python')
        print('s =', s)
        print('s.__thisclass__ =', s.__thisclass__)
        print('s.__self__ =', s.__self__)
        print('s.__self_class__ =', s.__self_class__)
        s.__init__(n)

    def number(self):
        return self.num

    def string(self):
        return self.ss

    def abs_meth(self):
        return 'abstractmethod'

try:
    from _bar import Bar_base
    print('you use c extension')
except ImportError as e:
    print('e =', e)
    print('you should get c extension')

class Bar(Bar_base):
    __doc__ = 'Bar.__doc__'

    def __init__(self, n):
        print('Bar.__init__()')
        print('Bar.__mro__ =', Bar.__mro__)
        # print('self.__dict__ =', self.__dict__)
        # print('id(self.__dict__) =', id(self.__dict__))
        s = super()
        print('super().__init__() in Bar pure Python')
        print('s =', s)
        print('s.__thisclass__ =', s.__thisclass__)
        print('s.__self__ =', s.__self__)
        print('s.__self_class__ =', s.__self_class__)
        print()
        s.__init__(n)
