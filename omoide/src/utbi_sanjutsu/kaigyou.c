/* kaigyou.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

#ifndef __KAIGYOU__
#define __KAIGYOU__
void kaigyou(char *p)
{
	while(*p != 0x0a && *p != '\0'){
		p++;
	}
	*p = '\0';
}
#endif

