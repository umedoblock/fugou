/* utbi_memory.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"
#include <stdlib.h>

int utbi_memory(unt **x, int n)
{
	extern int yousosuu;
	*x = (unt *)malloc(sizeof(unt) * yousosuu * n);
	if(*x==NULL){
		return 0;
	}else{
		return 1;
	}
}
