from par2 import *

p2 = Par2(4)
print('p2 =', p2)
print('p2.__sizeof__() =', p2.__sizeof__())
# print('p2.__dict__ =', p2.__dict__)
p2.v = 8
print('p2.v =', p2.v)
# print('p2.__dict__ =', p2.__dict__)
print('id(p2.__dict__) = 0x{:08x}'.format(id(p2.__dict__)))
print('p2.dump(p2) =')
p2.dump(p2, p2.__sizeof__())
# print(dir(p2))
print()
