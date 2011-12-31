/* utbi_fukusha_ui.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_fukusha_ui(unt *y, unt fukushamoto)
{
	extern int yousosuu;

	memset(y+1, 0, sizeof(unt)*(yousosuu-1));
	*y = fukushamoto;
	/*
	int i;

	*y = fukushamoto;
	y++;

	for(i=1; i<yousosuu; i++){
		*y = 0;
		y++;
	}
	*/
}
