/* utbi_put0x.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "utbi_sanjutsu.h"

void utbi_put0X(unt *x_p)
{
	int flg, i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	flg=0;
	for(i=(yousosuu - 1); i>=0; i--){
		printf("%08X", *x_p);
		x_p--;
	}
}

void utbi_put0x(unt *x_p)
{
	int flg, i;
	extern int yousosuu;

	x_p = x_p + (yousosuu - 1);

	flg=0;
	for(i=(yousosuu - 1); i>=0; i--){
		printf("%08x", *x_p);
		x_p--;
	}
}
