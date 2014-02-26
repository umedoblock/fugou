CC		= cc
OBJS	= test_rs.o rs.o ../libfugou.o ../assert_xxx.o
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
OPTIONS	= -D__TEST__
TARGET	= test_rs

test: $(OBJS)
	rm -f ./core
	$(CC) $(CFLAGS) $(OPTIONS) -o $(TARGET) $(OBJS)
	./test_rs
# rs.o: reed_solomon/rs.c reed_solomon/rs.h
# 	gcc -Wall -g -c reed_solomon/rs.c
#rs_test.o: rs.c
gdb:
	gdb ./$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) -c $<
rs.o: rs.h
test_rs.o: rs.h

.PHONY: clean
clean:
	$(RM) core $(OBJS)
