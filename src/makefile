# make の再帰利用
# http://www.ecoop.net/coop/translated/GNUMake3.77/make_5.jp.html#SEC49
CC		= cc
OBJS	= assert_xxx.o libfugou.o
CFLAGS	= -Wall -g

default: rs
	@echo this is top dir
test: $(OBJS)
	$(MAKE) test -C reed_solomon
rs: $(OBJS)
	$(MAKE) -C reed_solomon

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
assert_xxx.o: assert_xxx.h
libfugou.o: libfugou.h libfugou_base.h

.PHONY: clean
clean:
	$(RM) core $(OBJS)
	$(MAKE) clean -C reed_solomon
