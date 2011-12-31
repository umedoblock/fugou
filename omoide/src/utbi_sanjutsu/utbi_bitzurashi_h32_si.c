/* utbi_bitzurashi_h32_si.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdlib.h>
#include "utbi_sanjutsu.h"

#define __BITBITBIT__

#ifdef __BITBITBIT__
void utbi_bitzurashi_h32_si(unt *y, unt *x, int j)
{
	int i;
	extern int yousosuu;
	if(j){
		utbi_fukusha(y, x);
		for(i=yousosuu-1;i>=j;i--){
			*(y+i) = *(x+i-j);
		}
		for(;i>=0;i--){
			*(y+i) = 0;
		}
	}else{
		utbi_fukusha(y, x);
	}
}


#endif
#ifdef __BITBIT__
void utbi_bitzurashi_h32_si(unt *y, unt *x, int j)
{
	int i;
	extern int yousosuu;

	if(j){
		utbi_fukusha(y, x);

		y += (yousosuu - 1);/*配列の頭まできた*/
		x += (yousosuu - 1);

		for(i=0; i<(yousosuu-j); i++){
			*y = *(x - j);
			y--;
			x--;
		}
		for(i=(yousosuu-j); i<yousosuu; i++){
			*y = 0;
			y--;
		}
	}else{
		utbi_fukusha(y, x);
	}
}
#endif
