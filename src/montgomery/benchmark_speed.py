import sys
import timeit

use_c_extension = False
try:
    import _montgomery
    use_c_extension = True
    print('You use c extension for Montgomery().', file=sys.stderr)
except ImportError as e:
    print('e =', e, file=sys.stderr)
    print('You should get c extension for Montgomery().', file=sys.stderr)

print()

def run_performance(setUp, stmt, times=10):
    if not isinstance(setUp, str):
        setUp = '\n'.join(setUp)
    t = timeit.Timer(stmt, setUp)
    passed_time = t.timeit(number=times)
    print(stmt)
    print('{:.2f} usec/pass'.format(passed_time))
    print()

create_variables = '''
n = ('fa5688706cd7b1ae454c3065f7fe796bd27340a57cbc6708d07a5cd85149301'
     'a187bb7e13ae5f6aef664e8bfbb2eba351029d44ce0c68b3edb88c18d22b167'
     'e01099d6a94dbe75f21e1692413ac0602677e256696cd53c0ff30de29356277'
     '155c4541b31cc45f05b8ed05ee294e5fb556ac0c64f33b16939f3f55c92b86a'
     '3583')
n = int(n, 16)

x = ('ccd8009d78177f9236b3b5c7fbf25bc86bfec81d6d699e072387187cbefd659'
     'e8de6ae2786c783b52cc507b9540fefd7093d021ec7ee433062e002cc82cb14'
     '9a5d2f796771b65165f69df96dc441095c28b7efd59190fccdd79094fe19b05'
     '115a0a0bbd0ac7957c6f2c9734190fa45320d048de9d5ccc01994a71edf6f3e'
     'd471')
x = int(x, 16)

a = 2
'''

times_pow = 1
times_mul = 10 ** 5
times_gcdext = 1

import_module = ''
stmt = 'pow(a, x, n)'
run_performance([import_module, create_variables], stmt, times_pow)

setUp = '''
import montgomery_py3
''' + create_variables + '''
mng_py3 = montgomery_py3.Montgomery(n)
'''
stmt = 'mng_py3.mul(5031, 9417)'
run_performance(setUp, stmt, times_mul)

stmt = 'mng_py3.pow(a, x)'
run_performance(setUp, stmt, times_pow)

import_module = 'import montgomery_py3'
stmt = 'montgomery_py3.gcdext(n, x)'
run_performance([import_module, create_variables], stmt, times_gcdext)

setUp = '''
import _montgomery
''' + create_variables + '''
mng_c_ext = _montgomery.Montgomery(n)
'''

if use_c_extension:
    stmt = 'mng_c_ext.mul(a, x)'
    run_performance(setUp, stmt, times_mul)

    stmt = 'mng_c_ext.pow(a, x)'
    run_performance(setUp, stmt, times_pow)

    import_module = 'import _montgomery'
    stmt = '_montgomery.gcdext(n, x)'
    run_performance([import_module, create_variables], stmt, times_gcdext)
