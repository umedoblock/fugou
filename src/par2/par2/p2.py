import _par2
class Par2(_par2._Par2): pass

_p2 = _par2._Par2(4)
print('_p2 =', _p2)
print('_p2.__sizeof__() =', _p2.__sizeof__())
# print('_p2.__dict__ =', _p2.__dict__)
# _p2.v = 8
# print('_p2.v =', _p2.v)
# print('_p2.__dict__ =', _p2.__dict__)
print('_p2.dump(_p2) =')
_p2.dump(_p2, _p2.__sizeof__())
print()

p2 = Par2(4, division=15)
print('p2 =', p2)
print('p2.__sizeof__() =', p2.__sizeof__())
print('p2.__dict__ =', p2.__dict__)
p2.v = 8
print('p2.v =', p2.v)
print('p2.__dict__ =', p2.__dict__)
print('id(p2.__dict__) = 0x{:08x}'.format(id(p2.__dict__)))
print('p2.dump(p2) =')
p2.dump(p2, p2.__sizeof__())
print(dir(p2))
print()

class Foo(object): pass
foo = Foo()
p2.dump(foo, foo.__sizeof__())
