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

# AM_CFLAGS=-pg -Wall -O3
AM_CFLAGS=-pg -Wall -O3 -DDEBUG

lib_LTLIBRARIES = libfugou.la
libfugou_la_SOURCES = \
    camellia/camellia.h camellia/camellia.c \
    sha/sha.h sha/sha.c \
    slot/slot.h slot/slot.c \
    reed_solomon/rs.h reed_solomon/rs.c \
    reed_solomon/rsf.h reed_solomon/rsf.c \
    libfugou.c
# libfugou_la_LIBADD = \
#   sha/libsha.la \
#   camellia/libcamellia.la

# http://www.gnu.org/software/gnulib/manual/html_node/LD-Version-Scripts.html
# -Wl,--version-script,libfugou.map
# Currently, only GNU LD and the Solaris linker supports this.
# if HAVE_LD_VERSION_SCRIPT
    libfugou_la_LDFLAGS = LDFLAGS
    libfugou_la_LDFLAGS += -Wl,--version-script=libfugou.map
# else
##      libfugou_la_LDFLAGS = LDFLAGS
##      libfugou_la_LDFLAGS += -export-symbols-regex '^libfugou_.*'
##      libfugou_la_LDFLAGS += -export-symbols-regex '^rs_.*'
##      libfugou_la_LDFLAGS += -export-symbols-regex '^rsf_.*'
##      libfugou_la_LDFLAGS += -export-symbols-regex '^slot_.*'
##  #   libfugou_la_LDFLAGS += -export-symbols libfugou.sym
##  endif

bin_PROGRAMS = libfugou_sample
libfugou_sample_SOURCES = libfugou_sample.c libfugou.h
libfugou_sample_LDADD = libfugou.la

noinst_PROGRAMS = test_libfugou
test_libfugou_SOURCES = test_libfugou.c libfugou.h
test_libfugou_LDADD = libfugou.la
