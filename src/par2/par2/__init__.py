# Copyright 2011 梅どぶろく(umedoblock)

import sys

major = sys.version_info.major

from sys import modules
def do_unittest():
    return 'unittest' in modules

if do_unittest():
    __all__ = [ \
        'Par2', 'Par2Archive', 'Par2Error', \
        'matrix_to_bytes', 'bytes_to_matrix', \
        'Par2_base_PURE_PYTHON'
    ]
else:
    __all__ = [ \
        'Par2', 'Par2Archive', 'Par2Error', \
        'matrix_to_bytes', 'bytes_to_matrix' \
    ]

if major == 3:
    from par2.py3 import *
elif major == 2:
    from par2.py2 import *
else:
    raise('cannot use par2 in python{}'.format(major))

print('major is {}'.format(major))
print()
