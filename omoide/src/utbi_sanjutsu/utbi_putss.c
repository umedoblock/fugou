/* utbi_putss.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_putss(unt *ss_p)
{
	extern int yousosuu;
	int i;

	ss_p = ss_p + (yousosuu-1);
	for(i=(yousosuu - 1); i>=0; i--){
		putchar((0x000000ff & (*ss_p>>24) ));
		putchar((0x000000ff & (*ss_p>>16) ));
		putchar((0x000000ff & (*ss_p>>8) )) ;
		putchar((0x000000ff & (*ss_p) ));
		ss_p--;
	}
}
