/* utbi_shokika.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"
void utbi_shokika(unt *shokika)
{
	extern int yousosuu;
	memset(shokika, 0, sizeof(unt)*yousosuu);
/*	int i;

	for(i=0; i<yousosuu; i++){
		*(shokika+i) = 0;
	}
*/
}
