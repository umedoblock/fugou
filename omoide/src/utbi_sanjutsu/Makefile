# Makefile
# Copyright (C) 2008 梅どぶろく umedoblock

#マクロ定義部
CC		= cc
OBJS	= utbi_xor.o utbi_gcd.o utbi_putx.o utbi_irekae.o \
		  utbi_sosuu_miller_rabin.o utbi_fputss.o utbi_ret.o \
		  utbi_bitzurashi_h32_si.o xtoi.o utbi_futougou.o \
		  utbi_narabekae.o utbi_putss.o utbi_xtoi.o \
		  utbi_sa_ui.o utbi_sosuu_hantei.o utbi_bitzurashi_m32_si.o \
		  utbi_put0x.o utbi_sosuu_maeshori.o utbi_ior.o utbi_wa.o \
		  utbi_bitzurashi_m_si.o utbi_bekijouyo.o ctoi.o utbi_hanten.o \
		  utbi_amari.o utbi_sosuu_lehmann.o utbi_atamadase.o \
		  utbi_fukusha.o utbi_memory.o utbi_jouyo.o utbi_nijou.o \
		  utbi_seki.o utbi_sa.o utbi_futougou_ui.o \
		  utbi_bitzurashi_h_si.o utbi_dtoi.o utbi_wa_ui.o utbi_isg.o \
		  utbi_gcdext.o utbi_shokika.o utbi_itox.o utbi_fukusha_ui.o \
		  kaigyou.o utbi_shou.o utbi_file.o utbi_amari_ui.o \
		  utbi_datatoi.o bitkaiten_h_si.o utbi_shou_amari.o \
		  utbi_bitzurashi_h.o mazekoze.o utbi_ransuu.o shorijikan.o \
		  utbi_and.o utbi_bitset.o utbi_fputx.o utbi_bekijou.o \
		  utbi_bitzurashi_m.o sleep.o utbi_sosuu_seisei.o
INSPATH	= /usr/lib
TARGET	= libutbi_sanjutsu.so
OS_DEF	= -DDEBUG
OPTION	= -Wall -W -Werror -O -fPIC -g
LINKLIB	= 
MAJOR_V	= 0
MINOR_V	= 0.0
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
tes:
	rm ./tes
	gcc tes.c -o tes -lutbi_sanjutsu
