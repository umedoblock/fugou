import sys

try:
    from _montgomery import Montgomery
    from _montgomery import MontgomeryError
    from _montgomery import gcdext
    print('You use c extension for Montgomery().', file=sys.stderr)
except ImportError as e:
    from montgomery_py3 import Montgomery
    from montgomery_py3 import MontgomeryError
    from montgomery_py3 import gcdext
    print('e =', e, file=sys.stderr)
    print('You should get c extension for Montgomery().', file=sys.stderr)
