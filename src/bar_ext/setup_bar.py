from distutils.core import setup, Extension

module_bar = Extension('_bar', sources = ['bar/pybar.c'])

setup(  name = 'sample',
        description = 'This is a bar object package',
        ext_modules = [module_bar])
