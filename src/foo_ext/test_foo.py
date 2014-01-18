# Copyright 2011 梅濁酒(umedoblock)

import unittest
import sys, os
dname = os.path.dirname(os.path.abspath(__file__))
# print('dname =', dname)
libdir = os.path.join(dname, '..')
# print('libdir =', libdir)
sys.path.insert(0, libdir)

from foo import Foo_base
from foo import Foo_abstract
from foo import Foo

class TestFoo(unittest.TestCase):
    def test_bits(self):
        foo = Foo()
        print('CONST =', foo.CONST)
        print()

        self.assertTrue(isinstance(foo, Foo_abstract))
        self.assertTrue(isinstance(foo, Foo_base))
        self.assertTrue(isinstance(foo, Foo))
        self.assertEqual('string.', foo.string())
        self.assertEqual(8888, foo.number())

if __name__ == '__main__':
    unittest.main()
