include ../libfugou/Makefile.template

OBJS	= test_slot.o slot.o
TARGET	= test_slot

sample: slot.o sample_slot_of_rs.o ../reed_solomon/rs.o libfugou.a random_1048576.bin
	gcc -g sample_slot_of_rs.o slot.o ../reed_solomon/rs.o ../libfugou/libfugou.o -o sample_slot_of_rs
random_1048576.bin:
	dd if=/dev/urandom of=./random_1048576.bin bs=1024 count=1024
test: test_slot $(OBJS) libfugou.a
	rm -f ./core
	./test_slot
test_slot: $(OBJS) libfugou.a
	$(CC) $(CFLAGS) $(TEST_OPTIONS) -o $(TARGET) $(OBJS) $(AR_UPPER)
libfugou.a:
	cd ../libfugou && $(MAKE)
gdb:
	gdb ./$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
slot.o: slot.h
slot.h: libfugou_base.h
test_slot.o: slot.h libfugou.h assert_xxx.h

.PHONY: clean
clean:
	$(RM) core $(OBJS) $(TARGET) random_1048576.bin.*
