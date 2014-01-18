from distutils.core import setup, Extension

# module_camellia = Extension('camellia', sources = ['camellia/pycamellia.c'])
module_foo = \
    Extension('_foo',
        sources = ['foo/pyfoo.c'],
        extra_link_args=['-Wl,-soname,build/lib.linux-i686-3.2-pydebug/_foo.cpython-32dm.so'])
      # build/lib.linux-i686-3.2-pydebug/_foo.cpython-32dm.so
      # extra_compile_args=[''])

setup(  name = 'sample',
        version = '0.0',
        author = '梅濁酒(umedoblock)',
        author_email = 'umedoblock@gmail.com',
        url = 'empty',
        description = 'This is a foo object package',
        ext_modules = [module_foo])
