gf = [i for i in range(16)]
gfi = [i for i in range(16)]

for j in range(100000):
    if True:
        for i in range(16):
            c = gf[i] + gf[i]
            if c >= 16:
                c -= 16
            y = gfi[c]
    else:
        for i in range(16):
            c = i * i
            if c >= 16:
                c &= 15
