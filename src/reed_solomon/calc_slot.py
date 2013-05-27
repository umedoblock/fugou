primes = [
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
    157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
    239, 241, 251
]
# len(primes) = 54

# symbol_size = 3
# for prime in primes:
#     column_size = prime * symbol_size
#     if 1048576 % (column_size) == 1 or \
#        1048576 % (column_size) == (column_size - 1):
#        print('1048576 % column_size(={}, 3 * prime={}) = {}'.
#             format(column_size, prime, 1048576 % column_size))

divisions = (17, 32, 41)
symbol_size = 3
target_size = 1048576

for d in divisions:
    column_size = symbol_size * d
    remainder = target_size % column_size
    if remainder == 0:
        padding_size = 0
    else:
        padding_size = column_size - remainder
    rate = (1048576 + padding_size) / d
    print('(1048576 + {:3d}) / {} = {}, column_size = {:3d}, division = {}'.
           format(padding_size, d, rate, column_size, d))
# (1048576 +  35) / 17 = 61683.0, column_size =  51, division = 17
# (1048576 +  32) / 32 = 32769.0, column_size =  96, division = 32
# (1048576 + 122) / 41 = 25578.0, column_size = 123, division = 41

# 1048576 % prime=17 = 16
# 1048576 % prime=32 = 0
# 1048576 % prime=41 = 1
# (1048576 + 1) / 17 = 61681.0
# (1048576 + 0) / 16 = 65536.0
# (1048576 + 40) / 41 = 25576.0

# 1048576 % column_size(= 15, 3 * prime= 5) = 1
# 1048576 % column_size(= 33, 3 * prime=11) = 1
# 1048576 % column_size(= 93, 3 * prime=31) = 1
# 1048576 % column_size(=123, 3 * prime=41) = 1
