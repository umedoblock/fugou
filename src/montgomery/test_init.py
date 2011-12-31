import _montgomery
print('dir =')
dir(_montgomery)
print()
mng = _montgomery.Montgomery(101)
x, y, gcd = _montgomery.gcdext(101, 57)
print('x, y, gcd', x, y, gcd)
print('mng.r =', mng.r, 'mng.n =', mng.n)
