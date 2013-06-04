# >>> import itertools
# >>> it = itertools.combinations((4, 3, 2, 1), 2)
# >>> for i in it:
# ...  print(i)
# ...
# (4, 3)
# (4, 2)
# (4, 1)
# (3, 2)
# (3, 1)
# (2, 1)
# >>> tuple(itertools.combinations((4, 3, 2, 1), 2))
# ((4, 3), (4, 2), (4, 1), (3, 2), (3, 1), (2, 1))

import itertools
import functools
import operator

def code_speed(velocities):
    print('velocities =', velocities)
    len_velocities = len(velocities)
#   print('len_velocities =', len_velocities)

    up = functools.reduce(operator.mul, velocities)
    print('up =', up)

    down = 0.0
    v_combs = [v_comb for v_comb in itertools.combinations(velocities, len_velocities - 1)]
#   print('v_combs =', v_combs)
    for v_comb in v_combs:
        vs = functools.reduce(operator.mul, v_comb)
        down += vs
#       print('vs =', vs)
#       up *= v_comb[0] * v_
    print('down =', down)

    V = up / down
    print('V =', V)
    print()

    return V

if __name__ == '__main__':
    # #299: 符号化を重ねて行った場合の実効速度について。
    v1 = 30
    v2 = 50
    v3 = 70
    v4 = 110

    V = code_speed((v1, v1))
    V = code_speed((v1, v2, v3))
    V = code_speed((v1, v2, v3, v4))
    V = code_speed((v3, v3, v3, v3, v3, v3, v3))
