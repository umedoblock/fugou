CC		= cc
OBJS	= test_rs.o rs.o ../libfugou.o ../assert_xxx.o
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
OPTIONS	= -D__TEST__
TARGET	= test_rs

test: test_rs
	rm -f ./core
	./test_rs
test_rs: $(OBJS)
	$(CC) $(CFLAGS) $(OPTIONS) -o $(TARGET) $(OBJS)
# rs.o: reed_solomon/rs.c reed_solomon/rs.h
# 	gcc -Wall -g -c reed_solomon/rs.c
#rs_test.o: rs.c
gdb:
	gdb ./$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
rs.o: rs.h ../libfugou.h ../libfugou_base.h ../slot/slot.h
test_rs.o: rs.h ../libfugou.h ../libfugou_base.h ../slot/slot.h

.PHONY: clean
clean:
	$(RM) core $(OBJS) $(TARGET)
