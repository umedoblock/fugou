/* utbi_nijou.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_nijou(unt *mdr_nijou, unt *nijounomoto)
{
	extern int yousosuu;
	int i;
	unt flg;
	unt *nijounomoto_karimasu, *nijounomoto_karimasu_adr;
	unt *karimasuyon, *karimasuyon_adr;

	/*yousosuu個分のuntサイズのメモリを確保*/
	nijounomoto_karimasu = (unt *)calloc(yousosuu,sizeof(unt));
	if(nijounomoto_karimasu == NULL){
		printf("メモリ確保ならず　二乗基です残念。\n");
		exit(1);
	}
	nijounomoto_karimasu_adr = nijounomoto_karimasu;

	karimasuyon = (unt *)calloc(yousosuu,sizeof(unt));
	if(karimasuyon == NULL){
		printf("メモリ確保ならず。借りますよんです残念。\n");
		exit(1);
	}
	karimasuyon_adr = karimasuyon;

	utbi_fukusha(nijounomoto_karimasu, nijounomoto);
	utbi_fukusha(karimasuyon, nijounomoto);
	utbi_shokika(mdr_nijou);

	nijounomoto_karimasu += (yousosuu-1);

	for(i=yousosuu-1; i>=0; i--){
		if(*nijounomoto_karimasu){
			break;
		}
		nijounomoto_karimasu--;
	}
	flg = 0x00000001 << utbi_atamadase(*nijounomoto_karimasu);

	if((i == 0) && (*nijounomoto_karimasu == 0)){
		karimasuyon = karimasuyon_adr;
		nijounomoto_karimasu = nijounomoto_karimasu_adr;
		free(nijounomoto_karimasu);
		free(karimasuyon);
		return;
	}

	while(i>=0){
		for( ; flg; flg >>= 1){
			utbi_bitzurashi_h(mdr_nijou, mdr_nijou);
			if(flg & *nijounomoto_karimasu){
				utbi_wa(mdr_nijou, mdr_nijou, karimasuyon);
			}
		}
		flg = 0x80000000;
		nijounomoto_karimasu--;
		i--;
	}

	karimasuyon = karimasuyon_adr;
	nijounomoto_karimasu = nijounomoto_karimasu_adr;
	free(nijounomoto_karimasu);
	free(karimasuyon);
}
