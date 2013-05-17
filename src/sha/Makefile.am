# lib_LIBRARIES=libsha.a
# libsha_a_SOURCES=sha.c sha.h sha_private.h

lib_LTLIBRARIES=libsha.la
libsha_la_SOURCES=sha.c sha.h sha_private.h

# AM_CFLAGS=-A @CFLAGS@ -Z
# @CFLAGS@=-g -O2
# libsha_a_CFLAGS=-O3 -Wall -W -fPIC -pg -DNDEBUG -I$(SHA_INCLUDE)
# libsha_a_LDADD=libsha.a

bin_PROGRAMS=sha_sample
sha_sample_SOURCES=sha_sample.c sha.h sha_private.h
sha_sample_CFLAGS=-O3 -Wall -W -fPIC -pg -DNDEBUG -L. -I$(SHA_INCLUDE)
sha_sample_LDADD=-lsha

# bin_PROGRAMS=sha_test
# sha_test_SOURCES=sha_test.c sha.h sha_private.h
# sha_test_CFLAGS=-O3 -Wall -W -fPIC -pg -DNDEBUG -L. -I$(SHA_INCLUDE)
# sha_test_LDADD=-lsha
