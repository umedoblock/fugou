# -- Top-level Makefile.am --
# SUBDIRS = sha camellia . test
# SUBDIRS = sha camellia .
SUBDIRS = sha camellia .

noinst_LTLIBRARIES = libsha.la libcamellia.la

# -- sha/Makefile.am --
libsha_la_SOURCES = sha/sha.c sha/sha_private.h sha/sha.h

# -- camellia/Makefile.am --
# showing nested convenience libraries
libcamellia_la_SOURCES = \
  camellia/camellia.h camellia/camellia_cBytes.c camellia/camellia_destroy.c \
  camellia/camellia_keycopy.c camellia/camellia_subkey_generate.c \
  camellia/camellia_F.c camellia/camellia_cbc.c camellia/camellia_encrypt.c \
  camellia/camellia_keygen.c camellia/camellia_FL.c \
  camellia/camellia_debug.c camellia/camellia_hash.c \
  camellia/camellia_padding.c camellia/camellia_FL_1.c \
  camellia/camellia_decrypt.c camellia/camellia_hkaiten.c \
  camellia/camellia_putx.c

lib_LTLIBRARIES = libfugou.la
libfugou_la_SOURCES =
libfugou_la_LIBADD = \
  sha/libsha.la \
  camellia/libcamellia.la
