CC		= cc
OBJS	= assert_xxx.o libfugou.o reed_solomon/test_rs.o reed_solomon/rs.o
CFLAGS	= -Wall -g
OPTIONS	= -D__TEST__
TARGET	= test_rs

test_rs: $(OBJS)
	-ls rs.o && mv -f rs.o ./reed_solomon
	-ls test_rs.o && mv -f test_rs.o ./reed_solomon
	rm -f ./core
	$(CC) $(CFLAGS) $(OPTIONS) -o $(TARGET) $(OBJS)
	mv $(TARGET) ./reed_solomon
	./reed_solomon/test_rs
# rs.o: reed_solomon/rs.c reed_solomon/rs.h
# 	gcc -Wall -g -c reed_solomon/rs.c
#rs_test.o: rs.c
#	gcc -Wall -g -c -D__TEST__ reed_solomon/rs.c
#	gcc -Wall -g -D__TEST__ reed_solomon/test_rs.c ./assert_xxx.c libfugou.c reed_solomon/rs.c -o reed_solomon/test_rs
gdb:
	gdb ./reed_solomon/$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) -c $<
reed_solomon/rs.o: reed_solomon/rs.h
reed_solomon/test_rs.o: reed_solomon/rs.h
assert_xxx.o: assert_xxx.h
libfugou.o: libfugou.h libfugou_base.h

.PHONY: clean
clean:
	$(RM) $(OBJS)
