/* camellia_putx.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"
#include <stdio.h>

void camellia_putx(unsigned int *x, int n)
{
	int i;

	for(i=(n/32)-1;i>=0;i--){
		printf("%08x", *(x+i));
	}
}
