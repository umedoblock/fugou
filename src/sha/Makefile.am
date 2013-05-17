lib_LIBRARIES=libsha.a
libsha_a_SOURCES=sha.c sha.h sha_private.h
libsha_a_CFLAGS=-O3 -Wall -W -fPIC -pg -DNDEBUG -I$(SHA_INCLUDE)

bin_PROGRAMS=sha_sample
sha_sample_SOURCES=sha_sample.c sha.h sha_private.h
sha_sample_CFLAGS=-O3 -Wall -W -fPIC -pg -DNDEBUG -L. -I$(SHA_INCLUDE)
sha_sample_LDADD=-lsha
