# Copyright 2011 梅どぶろく(umedoblock)

import sys

__all__ = [ \
    'Par2', 'Par2Error', \
    'Par2Archive', 'Par2ArchiveError'
]

major = sys.version_info.major

if major == 3:
    from par2.py3 import *
elif major == 2:
    from par2.py2 import *
else:
    raise('cannot use par2 in python{}'.format(major))
