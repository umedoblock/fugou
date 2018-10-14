CC		= cc
OBJS	= test_rs.o rs.o expected_vm_of_rsx.o
AR_UPPER =../libfugou/libfugou.a
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
TEST_OPTIONS	= -D__TEST__
OPTIONS	= -D__TEST__ -I ../libfugou
TARGET	= test_rs
RANDOM_BIN = random_1048576.bin
RANDOM_BIN_INTEGRATE = random_1048576.bin.integrate
vpath %.h ../libfugou

test: test_rs libfugou.a
	rm -f ./core
	./test_rs
test_rs: $(OBJS) libfugou.a
	$(CC) $(CFLAGS) $(TEST_OPTIONS) -o $(TARGET) $(OBJS) $(AR_UPPER)
# rs.o: reed_solomon/rs.c reed_solomon/rs.h
# 	gcc -Wall -g -c reed_solomon/rs.c
#rs_test.o: rs.c
libfugou.a:
	cd ../libfugou && $(MAKE)
gdb:
	gdb ./$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
rs.o: rs.h libfugou.h libfugou_base.h ../slot/slot.h
test_rs.o: rs.h libfugou.h libfugou_base.h ../slot/slot.h

.PHONY: clean
clean:
	$(RM) core $(OBJS) $(OBJS_UPPER) $(TARGET) $(AR_UPPER) $(RANDOM_BIN)
