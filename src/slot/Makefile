CC		= cc
OBJS	= test_slot.o slot.o
AR_UPPER =../libfugou/libfugou.a
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
TEST_OPTIONS	= -D__TEST__
OPTIONS	= -D__TEST__ -I ../libfugou
TARGET	= test_slot
vpath %.h ../libfugou

test: test_slot $(OBJS) libfugou.a
	rm -f ./core
	./test_slot
test_slot: $(OBJS) libfugou.a
	$(CC) $(CFLAGS) $(TEST_OPTIONS) -o $(TARGET) $(OBJS) $(AR_UPPER)
libfugou.a:
	cd ../libfugou && $(MAKE)
gdb:
	gdb ./$(TARGET) ./core
sample: slot.o ../reed_solomon/rs.c sample_slot_of_rs.o libfugou.a
	gcc -g sample_slot_of_rs.c slot.c ../reed_solomon/rs.c ../libfugou.c -o sample_slot_of_rs

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
slot.o: slot.h libfugou.h libfugou_base.h
test_slot.o: slot.h libfugou.h libfugou_base.h

.PHONY: clean
clean:
	$(RM) core $(OBJS) $(TARGET)
