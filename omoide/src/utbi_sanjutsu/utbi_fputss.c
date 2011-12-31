/* utbi_fputss.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include "utbi_sanjutsu.h"

void utbi_fputss(FILE *fp, unt *ss_p)
{
	int i;
	extern int yousosuu;

	ss_p = ss_p + (yousosuu-1);

	for(i=(yousosuu - 1); i>=0; i--){
		fputc((0x000000ff) & (*ss_p>>24), fp);
		fputc((0x000000ff) & (*ss_p>>16), fp);
		fputc((0x000000ff) & (*ss_p>>8), fp);
		fputc((0x000000ff) & (*ss_p), fp);
		ss_p--;
	}
}
