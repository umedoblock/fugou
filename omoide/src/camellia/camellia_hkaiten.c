/* camellia_hkaiten.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "camellia.h"

void camellia_hkaiten(unt *hkaiten, int bLen)
{
	unt w;

	if(bLen&0xe0){
		w = *(hkaiten+3);
		*(hkaiten+3) = *(hkaiten+2);
		*(hkaiten+2) = *(hkaiten+1);
		*(hkaiten+1) = *(hkaiten+0);
		*(hkaiten+0) = w;
	}

	if(bLen&0x1f){
		hkaiten+=3;
		w = (*(hkaiten) >> (32-bLen));

		*(hkaiten) = (*(hkaiten) << bLen) | (*(hkaiten-1) >> (32-bLen));
		hkaiten--;
		*(hkaiten) = (*(hkaiten) << bLen) | (*(hkaiten-1) >> (32-bLen));
		hkaiten--;
		*(hkaiten) = (*(hkaiten) << bLen) | (*(hkaiten-1) >> (32-bLen));
		hkaiten--;
		*(hkaiten) = (*(hkaiten) << bLen) | (w);
	}

	/*ローカル変数の値を消去*/
	w = 0;
}

