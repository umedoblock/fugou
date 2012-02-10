import foo
fooo = None
from foo import Foo_abstract

fooo = foo.Foo(8)
print()
print('fooo =')
print(fooo)
print()
print('dir(fooo) =')
print(dir(fooo))

print()
print('delegates class')
s = super(foo.Foo, fooo)
print('s =', s)
print('s.__thisclass__ =', s.__thisclass__)
print('s..__self__ =', s.__self__)
print('s..__self_class__ =', s.__self_class__)

print()
print('delegates type')
print('s =', s)
s = super(foo.Foo_abstract, foo.Foo)
print('s.__thisclass__ =', s.__thisclass__)
print('s..__self__ =', s.__self__)
print('s..__self_class__ =', s.__self_class__)
