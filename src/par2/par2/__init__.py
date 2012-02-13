# Copyright 2011 梅どぶろく(umedoblock)

import sys

major = sys.version_info.major

__all__ = ['Par2', 'Par2Archive', 'Par2Error']

if major == 3:
    from par2.py3 import *
elif major == 2:
    from par2.py2 import *
else:
    raise('cannot use par2 in python{}'.format(major))

print('major is {}'.format(major))
print()
