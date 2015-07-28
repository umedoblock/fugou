from distutils.core import setup, Extension

ext_name = '_gcdext'
module_gcdext = Extension(ext_name, sources = ['ecc/_gcdext.c'],)

setup(name = 'tes',
      version = '0.0',
      author = '梅濁酒(umedoblock)',
      author_email = 'umedoblock@gmail.com',
      url = 'empty',
      description = 'This is a gcdext() package',
      ext_modules = [
          module_gcdext
      ])
