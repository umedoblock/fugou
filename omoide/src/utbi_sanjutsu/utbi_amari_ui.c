/* utbi_amari_ui.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

#define AKDAIEOAKFAIEFKAKD
#ifdef AKDAIEOAKFAIEFKAKDA
unt utbi_amari_ui(unt *a, unt n)
{
	unt y;
	/*マイナスn*/
	unt _n, t[15];
	unt tmp;
	int i, j;

	_n = ~n;
	_n++;
/*	_n %= n;*/
	printf("_n = 0x%x\n", _n);
	t[0] = _n;

	for(i=1;i<15;i++){
		t[i] = (t[i-1] + t[0]) % n;
	}

	y = 0;

	a += (yousosuu-1);
	for(i = (int)(yousosuu-1);i>=0;i--,a--){

		tmp = *a;
		y += tmp;

		if(y < tmp){
			y += _n;
		}

		y %= n;

		for(j=0;j<8*(sizeof(unt));j++){
			if(y&0x80000000){
				y <<= 1;
				y += _n;
				y %= n;
			}else{
				y <<= 1;
				y %= n;
			}
		}
	}

	return y;
}
#else
unt utbi_amari_ui(unt *a, unt n)
{
	unt *aa, *nn, *yy, *tmp;
	unt y;
	extern int yousosuu;

	utbi_memory(&tmp, 3);
	aa = tmp;
	nn = aa + yousosuu;
	yy = nn + yousosuu;
	utbi_fukusha(aa, a);
	utbi_fukusha_ui(nn, n);
	utbi_amari(yy, aa, nn);

	y = *yy;
	free(tmp);
	return y;
}
#endif

