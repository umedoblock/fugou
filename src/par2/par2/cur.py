import sys, time
for s in ('foo', 'barr', 'baz '):
    ss = s + ' ' * (80 - len(s))
    sys.stdout.write('\r%s' % ss)
    sys.stdout.flush()
    time.sleep(1)
