CC		= cc
OBJS	= test_slot.o slot.o
OBJS_UPPER =../libfugou.o ../assert_xxx.o
CFLAGS	= -Wall -g
#CFLAGS	= -Wall -g -O3
TEST_OPTIONS	= -D__TEST__
OPTIONS	= -D__TEST__
TARGET	= test_slot

test: test_slot $(OBJS)
	rm -f ./core
	./test_slot
test_slot: $(OBJS) $(OBJS_UPPER)
	$(CC) $(CFLAGS) $(TEST_OPTIONS) -o $(TARGET) $(OBJS) $(OBJS_UPPER)
gdb:
	gdb ./$(TARGET) ./core

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<
slot.o: slot.h ../libfugou.h ../libfugou_base.h ../slot/slot.h
test_slot.o: slot.h ../libfugou.h ../libfugou_base.h ../slot/slot.h

.PHONY: clean
clean:
	$(RM) core $(OBJS) $(TARGET)