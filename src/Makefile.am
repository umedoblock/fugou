# -- Top-level Makefile.am --
# SUBDIRS = sha camellia . test
# SUBDIRS = sha camellia .
# SUBDIRS = sha camellia .

# noinst_LTLIBRARIES = libsha.la libcamellia.la

# -- sha/Makefile.am --
# libsha_la_SOURCES = sha/sha.c sha/sha_private.h sha/sha.h

# -- camellia/Makefile.am --
# showing nested convenience libraries
# libcamellia_la_SOURCES = camellia/camellia.h camellia/camellia.c

CFLAGS=-pg -Wall -O3 -DDEBUG
CFLAGS=-pg -Wall -O3

lib_LTLIBRARIES = libfugou.la
libfugou_la_SOURCES = \
    camellia/camellia.h camellia/camellia.c \
    reed_solomon/rs.h reed_solomon/rs.c \
    sha/sha.h sha/sha.c \
    libfugou.c
#   reed_solomon/rsf.h reed_solomon/rsf.c \
# libfugou_la_LIBADD = \
#   sha/libsha.la \
#   camellia/libcamellia.la

bin_PROGRAMS = libfugou_sample
libfugou_sample_SOURCES = libfugou_sample.c libfugou.h
libfugou_sample_LDADD = libfugou.la

noinst_PROGRAMS = test_libfugou
test_libfugou_SOURCES = test_libfugou.c libfugou.h
test_libfugou_LDADD = libfugou.la
