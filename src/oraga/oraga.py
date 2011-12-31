import math

class Oraga:
    def __init__(self, n):
        self.n = n

        self.nbit = int(math.log(n, 2)) + 1
        self.r = 1 << self.nbit
        self.ffffffff = self.r - 1
        self._n = self.r - n

    def divmod(self, c):
        cbit = int(math.log(c, 2)) + 1
        count = cbit - self.nbit
        hi = c >> self.nbit
        low = c & self.ffffffff
        div = 0
        if hi > self.r:
            div, hi = self.divmod(hi)
      # print('div, hi = {}, {}'.format(div, hi))

      # print('cbit =', cbit)
      # print('nbit =', self.nbit)
      # print('count =', count)
      # print(' _n({:6}) = {:{}b}'.format(self._n, self._n, cbit))
      # print('  n({:6}) = {:{}b}'.format(self.n, self.n, cbit))
      # print('  c({:6}) = {:{}b}'.format(c, c, cbit))
      # print(' hi({:6}) = {:{}b}'.format(hi, hi, count))
      # low_ss = '{:0{}b}'.format(low, self.nbit)
      # print('low({:6}) = {:>{}}'.format(low, low_ss, cbit))
      # print('flg({:6}) = {:{}b}'.format(flg, flg, cbit))
      # print()

        for i in range(self.nbit):
          # print(' hi({:6d}) = {:{}b} before'.format(hi, hi, cbit))
            hi <<= 1
            div <<= 1
          # print(' hi({:6d}) = {:{}b} flg check'.format(hi, hi, cbit))
            if hi & self.r:
                div += 1
                hi &= self.ffffffff
                hi += self._n
                if hi >= self.n:
                    hi -= self.n
                    div += 1
          # print(' hi({:6d}) = {:{}b} after'.format(hi, hi, cbit))
          # print()

        if hi >= self.n:
            hi -= self.n
            div += 1
        if low >= self.n:
            low -= self.n
            div += 1
        ret = hi + low
        if ret >= self.n:
            ret -= self.n
            div += 1

        return div, ret


    def mul(self, a, b):
        c = a * b
        return self.divmod(c)

if __name__ == '__main__':
    a, b, n = 53, 47, 101
    div, mod = divmod(a * b, n)
    print('{} * {} / {} = {} * {} + {}'.format(a, b, n, div, n, mod))
    iihen = Oraga(n)
    ans = oraga.divmod(a * b)
    print('ans =', ans)
