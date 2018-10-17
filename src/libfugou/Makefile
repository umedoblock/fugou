# src
# |-- libfugou
# |-- reed_solomon
# |-- slot
# ...
# directory 階層が同じだから関係ないんか。
# libfugou dir 内の Makefile も
# reed_solomon, slot
# と同じように
# Makefile.template 使えるんだ。

include Makefile.template

OBJS = assert_xxx.o libfugou.o

libfugou.a: libfugou.o assert_xxx.o
	$(AR) $(ARFLAGS) $@ $?
libfugou.o: libfugou.h
assert_xxx.o: assert_xxx.h libfugou.h
libfugou.h: libfugou_base.h
assert_xxx.h: libfugou_base.h

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) core $(OBJS)
