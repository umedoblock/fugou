from abc import ABCMeta, abstractmethod, abstractproperty

class Bar_abstract(metaclass=ABCMeta):
    pass

try:
#   raise ImportError
    from _bar import Bar_base
    print('you use c extension')
  # print('_bar.Bar_base = 0x{:x}'.format(id(_bar.Bar_base)))
  # print(_bar.Bar_base)
  # print('dir(_bar.Bar_base) =')
  # print(dir(_bar.Bar_base))
  # continues = ['__abstractmethods__']
  # Bar_base = _bar.Bar_base
  # for name in dir(_bar.Bar_base):
  #     if name in continues:
  #         continue
  #     print('name =', name)
  #     print(getattr(_bar.Bar_base, name))
except ImportError as e:
    print('e =', e)
    print('you should get c extension')

    class Bar_base(Bar_abstract):
        pass

class Bar(Bar_base):
    pass
