/* utbi_bitzurashi_m_si.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "utbi_sanjutsu.h"

void utbi_bitzurashi_m_si(unt *y, unt *x, int j)
{
	int i;
	extern int yousosuu;
	if(j){
		utbi_fukusha(y, x);
		for(i=0; i < yousosuu-1; i++){
			*y = (*(x + 1) << (32-j)) | (*x >> j);
			x++;
			y++;
		}
		*y = *x >> j;
	}else{
		utbi_fukusha(y, x);
	}

}
