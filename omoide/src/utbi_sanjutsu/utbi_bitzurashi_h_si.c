/* utbi_bitzurashi_h_si.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_bitzurashi_h_si(unt *y, unt *x, int j)
{
	int i, k;
	extern int yousosuu;

	if(j){
		k = (j - 1) / (8*sizeof(unt));

		if(k){
			utbi_bitzurashi_h32_si(y, x, k);
		}
		y += (yousosuu-1);
		x += (yousosuu-1);


		for(i=(int)yousosuu-1;i>0;i--){
			*y = (*y << j) | (*(y-1) >> ((8*sizeof(unt))-j));
			y--;x--;
		}
		*y <<=j;
	}else{
		utbi_fukusha(y, x);
	}



}
