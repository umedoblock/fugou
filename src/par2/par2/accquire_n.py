import sys

def mul(x, y, mod):
    return x * y % mod

def checker(start, end):
    for mod in range(start, end):
        ss = ''
        appreciate = True
        for j in range(1, lim):
            s = set()
            for i in range(1, lim):
                m = mul(i, j, mod)
                m %= 16
                ss += '{:2d} '.format(m)
                if m in s:
                    appreciate = False
                    break
                s.add(m)
            ss += '\n'

        if appreciate:
            print('appreciate modulus', mod)
            print(ss)
            # print('appreciate modulus', mod, file=sys.stderr)
        else:
            # print('not appreciate modulus', mod)
            pass

lim = 16

checker(lim + 1, lim * 2)

# mod = 257
# for j in range(1, lim):
#     s = set()
#     for i in range(1, lim):
#         m = mul(i, j, mod)
#         m %= 256
#         print('{}'.format(m), end=' ')
#         # print('i =', i, m)
#         if m in s:
#             appreciate = False
#             break
#         s.add(m)
#     print()
