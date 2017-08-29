# name
#   name of the package 	short string 	(1)
# version
#   version of this release 	short string 	(1)(2)
# author
#   package author’s name 	short string 	(3)
# author_email
#   email address of the package author 	email address 	(3)
# maintainer
#   package maintainer’s name 	short string 	(3)
# maintainer_email
#   email address of the package maintainer 	email address 	(3)
# url
#   home page for the package 	URL 	(1)
# description
#   short, summary description of the package 	short string
# long_description
#   longer description of the package 	long string 	(5)
# download_url
#   location where the package may be downloaded 	URL 	(4)
# classifiers
#   a list of classifiers 	list of strings 	(4)
# platforms
#   a list of platforms 	list of strings
# license
#   license for the package 	short string 	(6)

from distutils.core import setup, Extension
import sys

# print('sys.argv =', sys.argv)
# print('type(sys.argv) =', type(sys.argv))
if '--pg' in sys.argv:
    suffix = '_pg'
    sys.argv.remove('--pg')
else:
    suffix = ''
# print('suffix =', suffix)

ext_name = '_par2' + suffix
module_par2 = \
    Extension(ext_name, sources=[
                        'par2/par2/pypar2.c',
                        'par2/par2/libpar2.c'
                       ],
              )

ext_name = '_gcdext' + suffix
module_gcdext = \
    Extension(ext_name, sources = ['ecc/ecc/_gcdext.c'],
              )

ext_name = '_montgomery' + suffix
module_montgomery = \
    Extension(ext_name, sources = ['montgomery/pymontgomery.c'])

ext_name = '_camellia' + suffix
module_camellia = \
    Extension(ext_name, sources = ['camellia/pycamellia.c',
    'camellia/camellia.c',
    ])

setup(  name = 'fugou',
        version = '8.0',
        author = '梅濁酒(umedoblock)',
        author_email = 'umedoblock@gmail.com',
        url = 'empty',
        description = 'This is a gcdext() package',
        ext_modules = [
            module_montgomery, module_gcdext, module_camellia
        ])
