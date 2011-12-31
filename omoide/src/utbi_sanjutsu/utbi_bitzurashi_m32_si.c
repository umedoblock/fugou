/* utbi_bitzurashi_m32_si.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_bitzurashi_m32_si(unt *y, unt *x, int j)
{
	int i;
	extern int yousosuu;


	if(j){

		utbi_fukusha(y, x);

		for(i=0; i<(yousosuu-j); i++){
			*y = *(y + j);
			y++;
		}

		for(i=(yousosuu-j); i<yousosuu; i++){
			*y = 0;
			y++;
		}
	}else{
		utbi_fukusha(y, x);
	}



}
