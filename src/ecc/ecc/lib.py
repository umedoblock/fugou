# author: 梅濁酒(umedoblock)
# Copyright 平成30年(2018)

import math

def collect_primes(n):
    primes = list()
    primes.append(2)
    for i in range(3, n + 1, 2):
        # print(i)
        dived = False
        sq = int(math.sqrt(i))
        for prime in primes:
            if prime > sq:
      #     if prime ** 2 > i:
                break
            if i % prime == 0:
                dived = True
        if not dived:
            primes.append(i)
    return primes

def is_prime(n):
    primes = collect_primes(n)
    return n in primes
