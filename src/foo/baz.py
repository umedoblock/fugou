from abc import ABCMeta, abstractmethod, abstractproperty

# class Baz_abstract(metaclass=ABCMeta):
class Baz_abstract(object):
    def __init__(self):
        self.v_abstract = 8
        self.p_abstract = 9
        dic = self.__dict__
        print('Baz_abstract id(__dict__) = {:x}'.format(id(dic)))
        print('Baz_abstract.__dict__ =', Baz_abstract.__dict__)
        print()


class Baz_base(Baz_abstract):
    '''help for Baz_base'''
    def __init__(self):
        super().__init__()
        self.v_base = 8
        self.p_base = 9
        dic = self.__dict__
        print('Baz_base id(__dict__) = {:x}'.format(id(dic)))
        print('Baz_base.__dict__ =', Baz_base.__dict__)
        print()

class Baz(Baz_base):
    '''help for Baz'''
    def __init__(self):
        super().__init__()
        self.v_baz = 8
        self.p_baz = 9
        dic = self.__dict__
        print('Baz id(__dict__) = {:x}'.format(id(dic)))
#       del self.v
        print('     Baz.__dict__ =', Baz.__dict__)
        print()

baz = Baz()
dic = baz.__dict__
print('baz id(__dict__) = {:x}'.format(id(dic)))
dir_ = dir(baz)
for d in dir(baz):
    pass
    # print('{:>16}'.format(d), getattr(baz, d))
print(dic)
print('dir_ =', type(dir_))
dic = baz.__dict__
print('baz id(__dict__) = {:x}'.format(id(dic)))
