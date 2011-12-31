/* sleep.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"
#include <time.h>

void sleep(double wtime)
{
	clock_t start, end;

	wtime /= 1000.0;

	start = clock();

	do{
		end = clock();
	}while(((double)(end - start) / CLOCKS_PER_SEC) < wtime);
}
