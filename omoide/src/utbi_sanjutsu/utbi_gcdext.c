/* utbi_gcdext.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include <stdio.h>
#include <stdlib.h>
#include "utbi_sanjutsu.h"

void utbi_gcdext(unt *mdr_gcd, unt *mdr_x, unt *mdr_y, unt *a_iroha, unt *b_nihohe)
{
	extern int yousosuu;
	unt *a_iroha_karimasu;
	unt *b_nihohe_karimasu;
	unt *x1_karimasu;
	unt *x2_karimasu;
	unt *y1_karimasu;
	unt *y2_karimasu;
	unt *z1_karimasu;
	unt *z2_karimasu;
	unt *shou_karimasu;
	unt *shou_ichijiteki;

	if(utbi_futougou_ui(a_iroha, 0) == 0 || utbi_futougou_ui(b_nihohe, 0) == 0){
		printf("私拡張ユークリッドの互除法です。\nx, yのどちらかが0でしたよ\n");
		getchar();
		exit(1);
	}

	a_iroha_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(a_iroha_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法aより\n");
		exit(1);
	}

	b_nihohe_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(b_nihohe_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法yより\n");
		exit(1);
	}

	x1_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(x1_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法x1より\n");
		exit(1);
	}

	x2_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(x2_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法x2より\n");
		exit(1);
	}

	y1_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(y1_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法y1より\n");
		exit(1);
	}

	y2_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(y2_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法y2より\n");
		exit(1);
	}

	z1_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(z1_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法z1より\n");
		exit(1);
	}

	z2_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(z2_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法z2より\n");
		exit(1);
	}

	shou_karimasu = (unt *)calloc(yousosuu, sizeof(unt));
	if(shou_karimasu == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法shouより\n");
		exit(1);
	}

	shou_ichijiteki = (unt *)calloc(yousosuu, sizeof(unt));
	if(shou_ichijiteki == NULL){
		printf("メモリ貸してくれない。拡張ユークリッドの互除法shou_ichijitekiより\n");
		exit(1);
	}

	utbi_shokika(a_iroha_karimasu);
	utbi_shokika(b_nihohe_karimasu);
	utbi_shokika(x1_karimasu);
	utbi_shokika(x2_karimasu);
	utbi_shokika(y1_karimasu);
	utbi_shokika(y2_karimasu);
	utbi_shokika(z1_karimasu);
	utbi_shokika(z2_karimasu);
	utbi_shokika(shou_karimasu);
	utbi_shokika(shou_ichijiteki);

	utbi_fukusha(a_iroha_karimasu, a_iroha);
	utbi_fukusha(b_nihohe_karimasu, b_nihohe);

	utbi_fukusha_ui(mdr_x, 1);
	utbi_fukusha_ui(mdr_y, 0);
	utbi_fukusha(mdr_gcd, a_iroha_karimasu);

	utbi_fukusha_ui(x1_karimasu, 0);
	utbi_fukusha_ui(y1_karimasu, 1);
	utbi_fukusha(z1_karimasu, b_nihohe_karimasu);

	while(utbi_futougou_ui(z1_karimasu, 0) > 0){

		utbi_shou_amari(shou_karimasu, z2_karimasu, mdr_gcd, z1_karimasu);

		utbi_seki(shou_ichijiteki, shou_karimasu, x1_karimasu);
		utbi_sa(x2_karimasu, mdr_x, shou_ichijiteki);

		utbi_seki(shou_ichijiteki, shou_karimasu, y1_karimasu);
		utbi_sa(y2_karimasu, mdr_y, shou_ichijiteki);

		utbi_fukusha(mdr_x, x1_karimasu);
		utbi_fukusha(mdr_y, y1_karimasu);
		utbi_fukusha(mdr_gcd, z1_karimasu);

		utbi_fukusha(x1_karimasu, x2_karimasu);
		utbi_fukusha(y1_karimasu, y2_karimasu);
		utbi_fukusha(z1_karimasu, z2_karimasu);

	}

	if(*(mdr_x+(yousosuu-1)) & 0x80000000){
		utbi_wa(mdr_x, mdr_x, b_nihohe_karimasu);
		utbi_sa(mdr_y, a_iroha_karimasu, mdr_y);
	}else{
		utbi_hanten(mdr_y, mdr_y);
		utbi_wa_ui(mdr_y, mdr_y, 1);
	}

	free(a_iroha_karimasu);
	free(b_nihohe_karimasu);
	free(x1_karimasu);
	free(x2_karimasu);
	free(y1_karimasu);
	free(y2_karimasu);
	free(z1_karimasu);
	free(z2_karimasu);
	free(shou_karimasu);
	free(shou_ichijiteki);
}
