# Makefile
# Copyright (C) 2008 梅どぶろく umedoblock

#マクロ定義部
CC		= cc
OBJS	= sha1.o sha256.o sha384.o sha512.o \
		  sha_func.o sha_puthash.o sha_random.o \
		  sha.o
INSPATH	= /usr/lib
TARGET	= libutbi_sha.so
OS_DEF	= -DDEBUG
OPTION	= -Wall -W -Werror -O -fPIC -g
LINKLIB	= 
MAJOR_V	= 0
MINOR_V	= 0.0
TEST	= test_sha
SONAME	= $(TARGET).$(MAJOR_V)
RLNAME	= $(SONAME).$(MINOR_V)
#生成規則部

$(TARGET): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $(RLNAME) $(OPTION) $(OBJS) $(LINKLIB)

#サフィックスルール(.c -> .o)
.c.o:
	$(CC) -c $(OPTION) $<
install:
	sudo cp $(RLNAME) $(INSPATH)
	cd $(INSPATH); pwd
	cd $(INSPATH); sudo chmod 744 $(RLNAME)
	cd $(INSPATH); sudo ln -s -f $(RLNAME) $(SONAME)
	cd $(INSPATH); sudo ln -s -f $(RLNAME) $(TARGET)
	sudo /sbin/ldconfig
#uninstall:
#	sudo rm -f $(INSPATH)/$(TARGET)*
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(RLNAME)
test:
	rm ./$(TEST)
	gcc $(TEST).c -o $(TEST) -lutbi_sha
