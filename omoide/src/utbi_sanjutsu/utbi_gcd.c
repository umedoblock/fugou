/* utbi_gcd.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_gcd(unt *mdr_gcd, unt *iroha_gcd, unt *nihohe_gcd)
{
	unt *iroha_gcd_karimasu, *iroha_gcd_karimasu_adr;
	unt *nihohe_gcd_karimasu, *nihohe_gcd_karimasu_adr;
	extern int yousosuu;

	iroha_gcd_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(iroha_gcd_karimasu == NULL){
		printf("メモリ貸してくれません。　いろは\n");
		exit(1);
	}
	iroha_gcd_karimasu_adr = iroha_gcd_karimasu;

	nihohe_gcd_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(nihohe_gcd_karimasu == NULL){
		printf("メモリ貸してくれません。　にほへ\n");
		exit(1);
	}
	nihohe_gcd_karimasu_adr = nihohe_gcd_karimasu;

	utbi_fukusha(iroha_gcd_karimasu, iroha_gcd);
	utbi_fukusha(nihohe_gcd_karimasu, nihohe_gcd);

	while(utbi_futougou_ui(nihohe_gcd_karimasu, 0) > 0){
		utbi_fukusha(mdr_gcd, nihohe_gcd_karimasu);
		utbi_amari(nihohe_gcd_karimasu, iroha_gcd_karimasu, mdr_gcd);
		utbi_fukusha(iroha_gcd_karimasu, mdr_gcd);
	}

	iroha_gcd_karimasu = iroha_gcd_karimasu_adr;
	nihohe_gcd_karimasu = nihohe_gcd_karimasu_adr;
	free(iroha_gcd_karimasu);
	free(nihohe_gcd_karimasu);
}
