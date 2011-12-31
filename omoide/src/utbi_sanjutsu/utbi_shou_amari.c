/* utbi_shou_amari.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_shou_amari(unt *mdr_shou, unt *mdr_amari, /**/unt *a, unt *x)
{
	extern int yousosuu;
	int i, j=0, atama, atama0, atama1;
	unt *a_karimasu, *a_karimasu_adr;
	unt *warukazu_shoki, *warukazu_shoki_adr;
	unt *hikukazu_shouyo_karimasu, *hikukazu_shouyo_karimasu_adr;
	unt *tmp;
	unt flg;

	if(!utbi_memory(&tmp, 3)){
		exit(1);
	}
	a_karimasu = a_karimasu_adr = tmp;
	warukazu_shoki_adr = warukazu_shoki = a_karimasu + yousosuu;
	hikukazu_shouyo_karimasu_adr = hikukazu_shouyo_karimasu = warukazu_shoki + yousosuu;

	utbi_fukusha(mdr_amari, a);
	utbi_fukusha(a_karimasu, a);
	utbi_fukusha(warukazu_shoki, x);
	utbi_fukusha(hikukazu_shouyo_karimasu, x);
	utbi_shokika(mdr_shou);

	if(utbi_futougou(a_karimasu, warukazu_shoki) < 0){
		free(tmp);
		return;
	}


	a_karimasu += (yousosuu -1);
	for(i=yousosuu-1; (*a_karimasu == 0)&&(i>0); i--){
		j--;
		a_karimasu--;
	}
	atama0 = utbi_atamadase(*a_karimasu);
	a_karimasu -= i;

	hikukazu_shouyo_karimasu += (yousosuu -1);
	for(i=yousosuu-1; (*hikukazu_shouyo_karimasu == 0)&&(i>0); i--){
		j++;
		hikukazu_shouyo_karimasu--;
	}
	atama1 = utbi_atamadase(*hikukazu_shouyo_karimasu);
	hikukazu_shouyo_karimasu -= i;

	atama = atama0 - atama1;

	if(atama > 0){
		;
	}else if(atama < 0){
		atama = 32 + atama;
		j--;
	}

	flg = 0x00000001 << atama;

	utbi_bitzurashi_h32_si(hikukazu_shouyo_karimasu, hikukazu_shouyo_karimasu, j);
	utbi_bitzurashi_h_si(hikukazu_shouyo_karimasu, hikukazu_shouyo_karimasu, atama);

	if(utbi_futougou(mdr_amari, hikukazu_shouyo_karimasu) < 0){
		utbi_bitzurashi_m(hikukazu_shouyo_karimasu, hikukazu_shouyo_karimasu);
		flg >>= 1;
		if(flg == 0){
			flg = 0x80000000;
			j--;
		}
	}
	/*ここで最初に引く状態になった。*/

	mdr_shou += j;
	/*商が最初は何番目のu_intに入ればいいのか指定*/

	while(utbi_futougou(mdr_amari, warukazu_shoki) >= 0){
		utbi_sa(mdr_amari, mdr_amari, hikukazu_shouyo_karimasu);
		*mdr_shou = *mdr_shou | flg;

		while(utbi_futougou(mdr_amari, hikukazu_shouyo_karimasu) < 0){
			utbi_bitzurashi_m(hikukazu_shouyo_karimasu, hikukazu_shouyo_karimasu);
			flg >>= 1;
			if(flg == 0){
				flg = 0x80000000;
				mdr_shou--;
			}
		}
	}

	free(tmp);
}
