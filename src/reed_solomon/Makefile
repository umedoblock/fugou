CC		= cc
OBJS	= test_rs.o rs.o expected_vm_of_rsx.o
OBJS_UPPER =../libfugou.o ../assert_xxx.o
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
TEST_OPTIONS	= -D__TEST__
OPTIONS	= -D__TEST__
TARGET	= test_rs

test: test_rs $(OBJS)
	rm -f ./core
	./test_rs
test_rs: $(OBJS) $(OBJS_UPPER)
	$(CC) $(CFLAGS) $(TEST_OPTIONS) -o $(TARGET) $(OBJS) $(OBJS_UPPER)
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
