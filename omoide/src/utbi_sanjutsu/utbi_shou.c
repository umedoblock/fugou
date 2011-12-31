/* utbi_shou.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_shou(unt *mdr_shou, unt *wararerukazu_shou, unt *warukazu_shou)
{
	extern int yousosuu;
	int i, j=0, atama, atama0, atama1;
	unt *wararerukazu_shou_karimasu, *wararerukazu_shou_karimasu_adr;
	unt *warukazu_shoki, *warukazu_shoki_adr;
	unt *shou_amari_karimasu, *shou_amari_karimasu_adr;
	unt *hikukazu_shou_karimasu, *hikukazu_shou_karimasu_adr;
	unt flg;

	if(utbi_memory(&wararerukazu_shou_karimasu, 4)){
		;
	}else{
		exit(1);
	}

	warukazu_shoki = wararerukazu_shou_karimasu + yousosuu;
	shou_amari_karimasu = warukazu_shoki + yousosuu;
	hikukazu_shou_karimasu = shou_amari_karimasu + yousosuu;

	wararerukazu_shou_karimasu_adr = wararerukazu_shou_karimasu;
	warukazu_shoki_adr = warukazu_shoki;
	shou_amari_karimasu_adr = shou_amari_karimasu;
	hikukazu_shou_karimasu_adr = hikukazu_shou_karimasu;

	utbi_fukusha(shou_amari_karimasu, wararerukazu_shou);
	utbi_fukusha(wararerukazu_shou_karimasu, wararerukazu_shou);
	utbi_fukusha(warukazu_shoki, warukazu_shou);
	utbi_fukusha(hikukazu_shou_karimasu, warukazu_shou);
	utbi_shokika(mdr_shou);

	if(utbi_futougou(wararerukazu_shou_karimasu, warukazu_shoki) < 0){
		wararerukazu_shou_karimasu = wararerukazu_shou_karimasu_adr;
		free(wararerukazu_shou_karimasu);
		return;
	}


	wararerukazu_shou_karimasu += (yousosuu -1);
	for(i=yousosuu-1; (*wararerukazu_shou_karimasu == 0)&&(i>0); i--){
		j--;
		wararerukazu_shou_karimasu--;
	}
	atama0 = utbi_atamadase(*wararerukazu_shou_karimasu);
	wararerukazu_shou_karimasu -= i;

	hikukazu_shou_karimasu += (yousosuu -1);
	for(i=yousosuu-1; (*hikukazu_shou_karimasu == 0)&&(i>0); i--){
		j++;
		hikukazu_shou_karimasu--;
	}
	atama1 = utbi_atamadase(*hikukazu_shou_karimasu);
	hikukazu_shou_karimasu -= i;

	atama = atama0 - atama1;

	if(atama > 0){
		;
	}else if(atama < 0){
		atama = 32 + atama;
		j--;
	}

	flg = 0x00000001 << atama;

	utbi_bitzurashi_h32_si(hikukazu_shou_karimasu, hikukazu_shou_karimasu, j);
	utbi_bitzurashi_h_si(hikukazu_shou_karimasu, hikukazu_shou_karimasu, atama);

	if(utbi_futougou(shou_amari_karimasu, hikukazu_shou_karimasu) < 0){
		utbi_bitzurashi_m(hikukazu_shou_karimasu, hikukazu_shou_karimasu);
		flg >>= 1;
		if(flg == 0){
			flg = 0x80000000;
			j--;
		}
	}
	/*ここで最初に引く状態になった。*/

	mdr_shou += j;
	/*商が最初は何番目のu_intに入ればいいのか指定*/

	while(utbi_futougou(shou_amari_karimasu, warukazu_shoki) >= 0){
		utbi_sa(shou_amari_karimasu, shou_amari_karimasu, hikukazu_shou_karimasu);
		*mdr_shou = *mdr_shou | flg;

		while(utbi_futougou(shou_amari_karimasu, hikukazu_shou_karimasu) < 0){
			utbi_bitzurashi_m(hikukazu_shou_karimasu, hikukazu_shou_karimasu);
			flg >>= 1;
			if(flg == 0){
				flg = 0x80000000;
				mdr_shou--;
			}
		}
	}

	wararerukazu_shou_karimasu = wararerukazu_shou_karimasu_adr;
	free(wararerukazu_shou_karimasu);
}
