/* utbi_bekijou.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_bekijou(unt *mdr_bekijou, unt *kisuu_bekijou, unt *bekisuu_bekijou)
{
	unt *kisuu_bekijou_karimasu, *kisuu_bekijou_adr;
	unt *bekisuu_bekijou_karimasu, *bekisuu_bekijou_adr;
	int i, atama;
	unt flg;
	unt *tmp;
	extern int yousosuu;

	if(utbi_memory(&tmp, 2)==0){
		exit(1);
	}
	kisuu_bekijou_karimasu = tmp;
	bekisuu_bekijou_karimasu = kisuu_bekijou_karimasu + yousosuu;

	kisuu_bekijou_adr = kisuu_bekijou_karimasu;
	bekisuu_bekijou_adr = bekisuu_bekijou_karimasu;

	utbi_fukusha_ui(mdr_bekijou, 1);
	utbi_fukusha(kisuu_bekijou_karimasu, kisuu_bekijou);
	utbi_fukusha(bekisuu_bekijou_karimasu, bekisuu_bekijou);

	bekisuu_bekijou_karimasu += (yousosuu - 1);

	for(i=(yousosuu-1); i>=0; i--){
		if(*bekisuu_bekijou_karimasu){
			break;
		}
		if(i==0){
			kisuu_bekijou_karimasu = kisuu_bekijou_adr;
			free(kisuu_bekijou_karimasu);
			return;
		}
		bekisuu_bekijou_karimasu--;
	}

	atama = utbi_atamadase(*bekisuu_bekijou_karimasu);
	flg = 0x00000001 << atama;

	while(i>=0){
		for( ; flg; flg >>= 1){
			utbi_nijou(mdr_bekijou, mdr_bekijou);
			if(flg & *bekisuu_bekijou_karimasu){
				utbi_seki(mdr_bekijou, mdr_bekijou, kisuu_bekijou_karimasu);
			}
		}
		flg = 0x80000000;
		bekisuu_bekijou_karimasu--;
		i--;
	}

	free(tmp);

}
