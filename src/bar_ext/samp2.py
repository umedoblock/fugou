import bar

baar = bar.Bar()
print('baar =')
print(baar)
print()
dic = baar.__dict__
print('id(baar.__dict__) = 0x{:x}'.format(id(dic)))
print('dir(baar) =')
print(dir(baar))
