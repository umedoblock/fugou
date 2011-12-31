/* utbi_fukusha.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"
void utbi_fukusha(unt *fukushasaki, unt *fukushamoto)
{
/*	static double t = 0.0;
	clock_t s, e;
	s = clock();
*/
/*
	if(fukushasaki == fukushamoto){
		putchar('\a');
		printf("fukushasaki == fukushamoto\n");
		getchar();
	}
*/
	extern int yousosuu;
	if(fukushasaki != fukushamoto)
		memcpy(fukushasaki, fukushamoto, sizeof(unt)*yousosuu);
/*  e = clock();

	t += (double)(e-s)/CLOCKS_PER_SEC;
	printf("utbi_fukusha() = %lf\n", t);
*/
/*	int i;

	for(i=0; i<yousosuu; i++){
		*fukushasaki = *fukushamoto;
		fukushasaki++;
		fukushamoto++;
	}
*/
}
